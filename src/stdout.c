/* stdout.c
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

#include "utils.h"
#include "view.h"

#include <stdio.h>

static void print_commit_diffs(const commit_t * commit)
{
	fprintf(stdout, "\t%zu file%c changed; " GREEN "+%zu" RESET " | " RED "-%zu" RESET "\n",
			commit->stats.files_changed,
			(commit->stats.files_changed > 1 ? 's': 0),
			commit->stats.lines_added,
			commit->stats.lines_removed);
}

static void print_commit_message(const commit_t * commit, const char *indent)
{
	fprintf(stdout, "%s| %s\n", indent, get_first_line(commit->msg).val);
}

static void print_stdout_grouped(const repository_t *repo, const indexes_t *indexes, const settings_t *settings)
{
	commit_t **const authored = indexes->authored;
	commit_t **const co_authored = indexes->co_authored;

	if (repo->history->n_authored == 0 && repo->history->n_co_authored == 0) { return; }

	fprintf(stdout, "Repository: %s\n", repo->name.val);

	if (repo->history->n_authored == 0) { goto co_authored; }

	fprintf(stdout, "Authored commits:\n");
	for (size_t n_c = 0; n_c < repo->history->n_authored; n_c++) {
		if (settings->print_header) {
			print_commit_message(authored[n_c], "\t");
		}
		fprintf(stdout, "\t| %s %s",
				authored[n_c]->hash.val,
				time_to_string(authored[n_c]->date).val);
		if (settings->show_diffs) {
			print_commit_diffs(authored[n_c]);
		}
		fprintf(stdout, "\n");
	}

co_authored:

	if (repo->history->n_co_authored == 0) { return; }

	fprintf(stdout, "Co-authored commits:\n");
	for (size_t n_c = 0; n_c < repo->history->n_co_authored; n_c++) {
		if (settings->print_header) {
			print_commit_message(co_authored[n_c], "\t");
		}
		fprintf(stdout, "\t| %s %s",
				co_authored[n_c]->hash.val,
				time_to_string(co_authored[n_c]->date).val);

		if (settings->show_diffs) {
			print_commit_diffs(co_authored[n_c]);
		}
		fprintf(stdout, "\n");
	}
}

static void print_stdout_list(const repository_t *repo, const indexes_t *indexes, const settings_t *settings)
{
	commit_t **const authored = indexes->authored;
	commit_t **const co_authored = indexes->co_authored;

	for (size_t n_c = 0; n_c < repo->history->n_authored; n_c++) {
		if (settings->print_header) {
			print_commit_message(authored[n_c], "");
		}
		fprintf(stdout, "| %s: %s %s [A]",
				repo->name.val,
				authored[n_c]->hash.val,
				time_to_string(authored[n_c]->date).val);
		
		if (settings->show_diffs) {
			print_commit_diffs(authored[n_c]);
		}
		fprintf(stdout, "\n");
	}

	for (size_t n_c = 0; n_c < repo->history->n_co_authored; n_c++) {
		if (settings->print_header) {
			print_commit_message(co_authored[n_c], "");
		}
		fprintf(stdout, "| %s: %s %s [C]",
				repo->name.val,
				co_authored[n_c]->hash.val,
				time_to_string(co_authored[n_c]->date).val);
		
		if (settings->show_diffs) {
			print_commit_diffs(co_authored[n_c]);
		}
		fprintf(stdout, "\n");
	}
}

void print_stdout(const repository_array_t *repos, settings_t settings)
{
	for (size_t i = 0; i < repos->count; i++) {
		const repository_t repo = repos->repositories[i];

		if (settings.grouped) {
			print_stdout_grouped(&repo, &repo.history->indexes, &settings);
		} else {
			print_stdout_list(&repo, &repo.history->indexes, &settings);
		}
	}

	fflush(stdout);
}
