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
#include "repo.h"
#include "settings.h"
#include "view.h"
#include "walk.h"

#include <stdlib.h>

int order_by_date_asc(const void* a, const void* b)
{
	commit_t **first = (commit_t **)a;
	commit_t **second = (commit_t **)b;

	if ((*first)->date == (*second)->date) { return 0; }
	return (*first)->date < (*second)->date ? -1 : 1;
}

int order_by_date_desc(const void* a, const void* b)
{
	return -order_by_date_asc(a,b);
}

static commit_t **get_commit_refs(const commit_arr_t *commit_arr, size_t commit_with_resp,
								  responsability_t resp, settings_t settings)
{
	commit_t **commits_with_resp = malloc(commit_with_resp * sizeof(commit_t *));
	for (size_t i = 0, n_resp = 0; i < commit_arr->count; i++) {
		if (commit_arr->commits[i].responsability == resp) {
			commits_with_resp[n_resp] = commit_arr->commits + i;
			n_resp++;
		}
	}
	if (settings.sorted) {
		qsort(commits_with_resp,
			  commit_with_resp,
			  sizeof(commit_t **),
			  settings.sort_order == ASC ? order_by_date_asc : order_by_date_desc);
	}

	return commits_with_resp;
}

static uint16_t build_indexes(repository_t *repo, settings_t settings)
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

static void print_output(const repository_array_t *repos, settings_t settings)
{
	switch (settings.output_mode) {
	case STDOUT:
		print_stdout(repos, settings);
		break;
	case LATEX:
		generate_latex_file(repos, settings);
		break;
	case HTML:
		generate_html_file(repos, settings);
		break;
	case JEKYLL:
		generate_markdown_file(repos, settings);
		break;
	default:
		fprintf(stderr, "corrupted output mode [%d]... stdout selected", settings.output_mode);
		break;
	}
}

return_code_t walk_through_repos(const repository_array_t *repos, settings_t settings)
{
	repository_t *repo;
	uint16_t ret;

	for (size_t i = 0; i < repos->count; i++) {
		repo = repos->repositories + i;
		repo->history = get_commit_history(repo->path, settings);
		ret = build_indexes(repo, settings);
		if (ret != OK) { return ret; }
	}
	
	print_output(repos, settings);
	
	return OK;
}
