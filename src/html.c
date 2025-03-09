/* html.c
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

#include "commit.h"
#include "repo.h"
#include "settings.h"
#include "utils.h"

#include <stdio.h>

#define COMMITS_DIV_STYLE "'display: flex; flex-direction: column; " \
						  "row-gap: 10px; padding-left: 2em;'"
#define COMMIT_ITEM_BORDER_STYLE "'border-left: black; "      \
								 "border-left-style: solid; " \
								 "border-left-width: 1px; "   \
								 "padding-left: 5px;'"
#define H3 "<h3 style='margin: 5px 0px;'><i>%s</i></h3>\n"
#define H2 "<h2 style='margin-bottom: 0px;'>%s</h2>\n"

static void print_commit_diffs(FILE *out, const commit_t * commit)
{
	fprintf(out, "%zu file%c changed "
				 "<span style='color:green;'>+%zu</span> "
				 "|  <span style='color:red;'>-%zu</span>\n",
			commit->stats.files_changed,
			(commit->stats.files_changed > 1 ? 's': 0),
			commit->stats.lines_added,
			commit->stats.lines_removed);
}

static void print_commit_message(FILE *out, const commit_t * commit)
{
	fprintf(out, "<span>%s</span>\n", get_first_line(commit->msg).val);
}

static void generate_html_file_grouped(FILE *out,
									   const repository_t *repo,
									   const indexes_t *indexes,
									   const settings_t *settings)
{
	commit_t **const authored = indexes->authored;
	commit_t **const co_authored = indexes->co_authored;

	if (repo->history->n_authored == 0 && repo->history->n_co_authored == 0) { return; }

	fprintf(out, H2, repo->name.val);
	
	if (repo->history->n_authored == 0) { goto co_authored; }

	fprintf(out, H3 "\n\n<div style=" COMMITS_DIV_STYLE ">\n",
			"Authored");
	
	for (size_t n_c = 0; n_c < repo->history->n_authored; n_c++) {
		fprintf(out, "<div>\n");
		if (settings->print_header) {
			print_commit_message(out, authored[n_c]);
		}
		fprintf(out, "<div style='font-size: %s;'>(%s) <a href='%s' target='_blank'>%s</a> ",
				settings->print_header ? "11pt" : "unset",
				time_to_string(authored[n_c]->date).val,
				get_github_url(repo->url, authored[n_c]->hash).val,
				authored[n_c]->hash.val);
		if (settings->show_diffs) {
			print_commit_diffs(out, authored[n_c]);
		}
		fprintf(out, "</div>");
		fprintf(out, "</div>\n");
	}

	fprintf(out, "</div>\n");

co_authored:
	
	if (repo->history->n_co_authored == 0) { return; }

	fprintf(out, H3 "\n\n<div style=" COMMITS_DIV_STYLE ">\n",
			"Co-authored");
	
	for (size_t n_c = 0; n_c < repo->history->n_co_authored; n_c++) {
		fprintf(out, "<div>\n");
		if (settings->print_header) {
			print_commit_message(out, co_authored[n_c]);
		}
		fprintf(out, "<div style='font-size: %s;'>(%s) <a href='%s' target='_blank'>%s</a> ",
				settings->print_header ? "11pt" : "unset",
				time_to_string(co_authored[n_c]->date).val,
				get_github_url(repo->url, co_authored[n_c]->hash).val,
				co_authored[n_c]->hash.val);
		if (settings->show_diffs) {
			print_commit_diffs(out, co_authored[n_c]);
		}
		fprintf(out, "</div>");
		fprintf(out, "</div>\n");
	}

	fprintf(out, "</div>\n");
}

static void generate_html_file_list(FILE *out,
									const repository_t *repo,
									const indexes_t *indexes,
									const settings_t *settings)
{
	commit_t **const authored = indexes->authored;
	commit_t **const co_authored = indexes->co_authored;

	for (size_t n_c = 0; n_c < repo->history->n_authored; n_c++) {
		fprintf(out, "<div style=" COMMIT_ITEM_BORDER_STYLE ">\n");
		if (settings->print_header) {
			print_commit_message(out, authored[n_c]);
		}
		fprintf(out, "<div>%s: <a href='%s' target='_blank'>%s</a> (%s) [A] ",
				repo->name.val,
				get_github_url(repo->url, authored[n_c]->hash).val,
				authored[n_c]->hash.val,
				time_to_string(authored[n_c]->date).val);
		if (settings->show_diffs) {
			print_commit_diffs(out, authored[n_c]);
		}
		fprintf(out, "</div>");
		fprintf(out, "</div>\n");
	}

	for (size_t n_c = 0; n_c < repo->history->n_co_authored; n_c++) {
		fprintf(out, "<div style=" COMMIT_ITEM_BORDER_STYLE ">\n");
		if (settings->print_header) {
			print_commit_message(out, co_authored[n_c]);
		}
		fprintf(out, "<div>%s: <a href='%s' target='_blank'>%s</a> (%s) [C] ",
				repo->name.val,
				get_github_url(repo->url, co_authored[n_c]->hash).val,
				co_authored[n_c]->hash.val,
				time_to_string(co_authored[n_c]->date).val);
		if (settings->show_diffs) {
			print_commit_diffs(out, co_authored[n_c]);
		}
		fprintf(out, "</div>");
		fprintf(out, "</div>\n");
	}
}

void generate_html_file(const repository_array_t *repos, settings_t settings)
{
	FILE *out = fopen(settings.output.val, "w");

	fprintf(out, "<!-- This file is automatically generated by TUR -->\n\n");

	fprintf(out, "<center><h1>Commits</h1></center>\n");

	if (!settings.grouped) {
		fprintf(out, "<div style=" COMMITS_DIV_STYLE ">\n");
	}

	for (size_t i = 0; i < repos->count; i++) {
		const repository_t repo = repos->repositories[i];

		if (settings.grouped) {
			generate_html_file_grouped(out, &repo, &repo.history->indexes, &settings);
		} else {
			generate_html_file_list(out, &repo, &repo.history->indexes, &settings);
		}
	}

	if (!settings.grouped) {
		fprintf(out, "</div>\n");
	}

	fclose(out);
}
