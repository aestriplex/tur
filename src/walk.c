/* walk.c
 * -----------------------------------------------------------------------
 * Copyright (C) 2025  Matteo Nicoli
 *
 * This file is part of TUR.
 *
 * TUR is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "codes.h"
#include "commit.h"
#include "log.h"
#include "repo.h"
#include "settings.h"
#include "view.h"
#include "walk.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static thread_pool_t pool;

static int order_by_date_asc(const void* a, const void* b)
{
	commit_t **first = (commit_t **)a;
	commit_t **second = (commit_t **)b;

	if ((*first)->date == (*second)->date) { return 0; }
	return (*first)->date < (*second)->date ? -1 : 1;
}

static int order_by_date_desc(const void* a, const void* b)
{
	return -order_by_date_asc(a,b);
}

static commit_t **get_commit_refs(const commit_arr_t *commit_arr, size_t commit_with_resp,
								  responsability_t resp, const settings_t *settings)
{
	commit_t **commits_with_resp = malloc(commit_with_resp * sizeof(commit_t *));
	for (size_t i = 0, n_resp = 0; i < commit_arr->count; i++) {
		if (commit_arr->commits[i].responsability == resp) {
			commits_with_resp[n_resp] = commit_arr->commits + i;
			n_resp++;
		}
	}
	if (settings->sorted) {
		qsort(commits_with_resp,
			  commit_with_resp,
			  sizeof(commit_t *),
			  settings->sort_order == ASC ? order_by_date_asc : order_by_date_desc);
	}

	return commits_with_resp;
}

static uint16_t build_indexes(repository_t *repo, const settings_t *settings)
{
	repo->history->indexes.authored = get_commit_refs(&repo->history->commit_arr,
													  repo->history->n_authored,
													  AUTHORED, settings);
	repo->history->indexes.co_authored = get_commit_refs(&repo->history->commit_arr,
														 repo->history->n_co_authored,
														 CO_AUTHORED, settings);
	
	if (!repo->history->indexes.authored
		|| !repo->history->indexes.co_authored) { return INDEX_ALLOCATION_ERROR; }

	return OK;
}

static void print_output(const repository_array_t *repos, const settings_t *settings)
{
	if (settings->output_mode == STDOUT) {
		print_stdout(repos, settings);
		return;
	}

	FILE *out = fopen(settings->output.val, "w");
	if (!out) {
		(void)log_err("print_output: cannot open file: %s\n", settings->output.val);
		return;
	}

	switch (settings->output_mode) {
	case LATEX:
		generate_latex_file(out, repos, settings);
		break;
	case HTML:
		generate_html_file(out, repos, settings);
		break;
	case JEKYLL:
		generate_markdown_file(out, repos, settings);
		break;
	default:
		(void)log_err("corrupted output mode [%d]... stdout selected", settings->output_mode);
		break;
	}

	fclose(out);
}

static void *walk_repo(void* arg)
{
	size_t max_name_len = *(size_t *)arg;
	thread_worker_t *worker;

	while (1) {
		pthread_mutex_lock(&pool.current_worker_lock);
		if (pool.current_worker >= pool.n_workers) {
			pthread_mutex_unlock(&pool.current_worker_lock);
			break;
		}
		worker = pool.workers + pool.current_worker;
		pool.current_worker++;
		pthread_mutex_unlock(&pool.current_worker_lock);

		const char *branch_name = worker->repo->branches ? worker->repo->branches->strings[0].val : NULL;

		worker->repo->history = get_commit_history(worker->repo->path, branch_name, pool.settings);
		if (!worker->repo->history) {
			worker->ret = RUNTIME_MALLOC_ERROR;
			(void)log_err("walk_repo: cannot retrieve commit history for %s\n", worker->repo->name.val);
			continue;
		}
		worker->ret = build_indexes(worker->repo, pool.settings);
		(void)log_info("%-5lu commits in %-*s  +%lu | -%lu  ~%s\n",
					   worker->repo->history->commit_arr.count,
					   max_name_len,
					   worker->repo->name.val,
					   worker->repo->history->tot_lines_added,
					   worker->repo->history->tot_lines_removed,
					   branch_name ? branch_name : "HEAD");
	}
	
	return NULL;
}

static return_code_t init_thread_pool(const repository_array_t *repos, const settings_t *settings)
{
	pool.threads = malloc(settings->n_threads * sizeof(pthread_t));
	if (!pool.threads) { goto err; }
	pool.workers = malloc(repos->count * sizeof(thread_worker_t));
	if (!pool.workers) { goto err; }
	pool.settings = settings;
	pool.n_threads = settings->n_threads;
	pool.n_workers = repos->count;
	pool.current_worker = 0;
	pthread_mutex_init(&pool.current_worker_lock, NULL);

	return OK;

err:
	(void)log_err("init_thread_pool: memory allocation for thread pool failed.");
	return RUNTIME_MALLOC_ERROR;
}

return_code_t walk_through_repos(const repository_array_t *repos, const settings_t *settings)
{
	return_code_t ret = OK;
	size_t max_name_len = repos->max_name_len;

	ret = init_thread_pool(repos, settings);
	if (ret != OK) { return RUNTIME_MALLOC_ERROR; }

	(void)log_info("Created thread pool [size %lu]\n", pool.n_threads);

	__sync_synchronize();

	for (size_t i = 0; i < repos->count; i++) {
		pool.workers[i] = (thread_worker_t) {
			.repo = repos->repositories + i,
			.ret = OK
		};
	}

	for (size_t i = 0; i < pool.n_threads; i++) {
		if (pthread_create(pool.threads + i, NULL, walk_repo, &max_name_len) != 0) {
			(void)log_err("walk_through_repos: cannot create thread #%zu\n", i);
			return RUNTIME_THREAD_CREATE_ERROR;
		}
	}

	for (size_t i = 0; i < pool.n_threads; i++) {
		pthread_join(pool.threads[i], NULL);
	}

	for (size_t i = 0; i < pool.n_workers; i++) {
		if (pool.workers[i].ret != OK) {
			(void)log_err("walk_through_repos: worker #%zu failed with error code %d",
						  i, pool.workers[i].ret);
			return pool.workers[i].ret;
		}
	}

	(void)log_info("--------------\n");

	print_output(repos, settings);

	return ret;
}
