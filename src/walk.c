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

#include "cache.h"
#include "codes.h"
#include "commit.h"
#include "editor.h"
#include "log.h"
#include "repo.h"
#include "settings.h"
#include "view.h"
#include "walk.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef int (*ord_fn_t) (const void* a, const void* b);

#define REPO_STAT_LOG_STR "%-5lu commits in %-*s  +%lu | -%lu  " \
						  "[AVG +%.2f | -%.2f]  ~%s\n"
#define FLOAT_AVG(x,y) ((float) ((float) x / (y)))

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

static commit_t **get_commit_refs(const commit_arr_t *commit_arr,
								  size_t commit_with_resp,
								  responsability_t resp,
								  const settings_t *settings)
{
	commit_t **commits_with_resp = malloc(commit_with_resp * sizeof(commit_t *));
	for (size_t i = 0, n_resp = 0; i < commit_arr->len; i++) {
		commit_t *commit = commit_array_get(commit_arr, i);
		if (commit->responsability == resp) {
			commits_with_resp[n_resp] = commit;
			n_resp++;
		}
	}
	if (settings->sorted) {
		ord_fn_t ord_fn = settings->sort_order == ASC
						  ? order_by_date_asc
						  : order_by_date_desc;
		qsort(commits_with_resp,
			  commit_with_resp,
			  sizeof(commit_t *),
			  ord_fn);
	}

	return commits_with_resp;
}
static bool non_cached_non_inter(const settings_t *settings)
{
	return settings->no_cache && !settings->interactive;
}

static bool cached_or_inter(const settings_t *settings)
{
	return !non_cached_non_inter(settings);
}

static return_code_t build_indexes(repository_t *repo,
								   const settings_t *settings)
{
	commit_t **authored = get_commit_refs(repo->history->commit_arr,
										  repo->history->n_authored,
										  AUTHORED, settings);
	commit_t **co_authored = get_commit_refs(repo->history->commit_arr,
											 repo->history->n_co_authored,
											 CO_AUTHORED, settings);

	repo->history->indexes.authored = authored;
	repo->history->indexes.co_authored = co_authored;

	const indexes_t *idx = &repo->history->indexes; 
	if (!idx->authored || !idx->co_authored) { return INDEX_ALLOCATION_ERROR; }

	return OK;
}

static void print_output(const repository_array_t *repos,
						 const settings_t *settings)
{
	if (settings->output_mode == STDOUT) {
		print_stdout(repos, settings);
		return;
	}

	FILE *out = fopen(settings->output.val, "w");
	if (!out) {
		(void)log_err("print_output: cannot open file: %s\n",
					  settings->output.val);
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
		(void)log_err("corrupted output mode [%d]... stdout selected",
					  settings->output_mode);
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

		const char *branch_name = worker->repo->branches
								  ? str_array_get(worker->repo->branches, 0).val
								  : NULL;

		worker->repo->history = get_commit_history(worker->repo->path,
												   branch_name, pool.settings);
		if (!worker->repo->history) {
			worker->ret = RUNTIME_MALLOC_ERROR;
			(void)log_err("walk_repo: cannot retrieve commit history for %s\n",
						  worker->repo->name.val);
			continue;
		}
		worker->ret = build_indexes(worker->repo, pool.settings);

		/* Print log with stats */
		const size_t n_commits = worker->repo->history->commit_arr->len;
		const size_t lines_added = worker->repo->history->tot_lines_added;
		const size_t lines_removed = worker->repo->history->tot_lines_removed;
		(void)log_info(REPO_STAT_LOG_STR,
					   n_commits,
					   max_name_len,
					   worker->repo->name.val,
					   lines_added,
					   lines_removed,
					   FLOAT_AVG(lines_added, n_commits),
					   FLOAT_AVG(lines_removed, n_commits),
					   branch_name ? branch_name : "HEAD");
	}
	
	return NULL;
}

static return_code_t init_thread_pool(const repository_array_t *repos,
									  const settings_t *settings)
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

static return_code_t cache_commit_list(const repository_array_t *repos,
									   const settings_t *settings)
{
	return_code_t ret = OK;

	if (cached_or_inter(settings)) {
		if (settings->force || !commit_file_exists()) {
			/* We have to create or overwrite the commits file */
			ret = write_repos_on_file(repos);
			if (ret != OK) { return ret; }
		}
	}

	if (settings->interactive) {
		ret = choose_commits_through_editor(settings);
		if (ret != OK) { return ret; }
	}

	return ret;
}

return_code_t walk_through_repos(const repository_array_t *repos,
								 const settings_t *settings)
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

	/* The indexes are built following these rationale:
	 *     - If no_cache == 1 && interactive == 0, then .tur/commits is ignored;
	 *     - If no_cache == 1 && interactive == 1, then .tur/commits is temporarily written,
	 *       and removed at the end of this function;
	 *     - If no_cache == 0 && interactive == 0, then .tur/commits is written with the indexes
	 *       built by this function;
	 *     - If no_cache == 0 && interactive == 1, then .tur/commits is written with the indexes
	 *       modified by the user in the text editor (interctive mode). If a file .tur/commits
	 *       is already present, then
	 *           * if force == 1, then the index is recalculated and the file overwritten;
	 *           * otherwise, the file is loaded as is and the index is not recalculated.
	 */
	ret = cache_commit_list(repos, settings);
	if (ret != OK) { goto print_and_exit; }

	if (cached_or_inter(settings)) {
		ret = rebuild_indexes(repos);
		if (ret != OK) { goto print_and_exit; }
	}

	if (settings->no_cache && commit_file_exists()) {
		(void)log_info("Removing temporary commit file `%s`...\n",
					   COMMITS_FILE);
		ret = delete_commits_file();
		if (ret != OK) {
			(void)log_err("Cannot delete temporary commit file `%s`...\n", COMMITS_FILE);
		}
	}

print_and_exit:
	print_output(repos, settings);

	return ret;
}
