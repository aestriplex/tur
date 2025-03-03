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

static int compare_commits(const void* a, const void* b)
{
	commit_t *first = (commit_t *)a;
	commit_t *second = (commit_t *)b;

	if (!first || !second) { return 0; }

	if (first->date == second->date) { return 0; }

	return first->date < second->date ? -1 : 1;
}

static commit_refs_t *init_commit_refs(size_t n_commits)
{
	commit_refs_t *c_r = malloc(sizeof(commit_refs_t));
	c_r->commits = malloc(n_commits * sizeof(commit_t *));
	c_r->n_commits = n_commits;
	return c_r;
}

static commit_refs_t *get_commit_refs(commit_list_t *commits, size_t commit_with_resp,
									  responsability_t resp, settings_t settings)
{
	commit_refs_t *commits_with_resp = init_commit_refs(commit_with_resp);
	commit_list_t *current = commits;
	size_t i = 0;
	while (current) {
		if (current->commit.responsability == resp) {
			commits_with_resp->commits[i] = &current->commit;
			i++;
		}
		current = current->parent;
	}

	if (settings.sorted) {
		qsort(commits_with_resp->commits, commit_with_resp, sizeof(commit_t), compare_commits);
	}

	return commits_with_resp;
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
		fprintf(stderr, "Output mode not implemented yet...\n");
		break;
	default:
		fprintf(stderr, "corrupted output mode [%d]... stdout selected", settings.output_mode);
		break;
	}
}

return_code_t walk_through_repos(const repository_array_t *repos, settings_t settings)
{
	repository_t *repo;

	for (size_t i = 0; i < repos->count; i++) {
		repo = repos->repositories + i;
		repo->history = get_commit_history(repo->path, settings);
		repo->history->authored = get_commit_refs(repo->history->list, repo->history->n_authored, AUTHORED, settings);
		repo->history->co_authored = get_commit_refs(repo->history->list, repo->history->n_co_authored, CO_AUTHORED, settings);
	}

	print_output(repos, settings);
	
	return OK;
}
