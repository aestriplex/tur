/* latex.c
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

#define LIST_ITEMS_SPACING "\\setlength\\itemsep{1em}"

static void print_commit_diffs(FILE *out, const commit_t * commit)
{
	fprintf(out, "\\\\%zu file%c changed "
				 "\\textcolor{teal}{+%zu} "
				 "$~\\vert{}~$ \\textcolor{red}{-%zu}\n",
			commit->stats.files_changed,
			(commit->stats.files_changed > 1 ? 's': ASCII_SPACE),
			commit->stats.lines_added,
			commit->stats.lines_removed);
}

static void print_commit_message(FILE *out, const commit_t * commit)
{
	fprintf(out, "%s\\\\ \n", escape_special_chars(get_first_line(commit->msg)).val);
}

static void generate_latex_file_grouped(FILE *out,
										const repository_t *repo,
										const indexes_t *indexes,
										const settings_t *settings)
{
	commit_t **const authored = indexes->authored;
	commit_t **const co_authored = indexes->co_authored;

	if (repo->history->n_authored == 0 && repo->history->n_co_authored == 0) { return; }

	fprintf(out, "\n\n\\subsection{%s}\n\\label{subsec:%s}\n", repo->name.val, repo->name.val);
		
	if (repo->history->n_authored == 0) { goto co_authored; }

	fprintf(out, "\n\\turtexpar{Authored}\n\\label{par:%s-authored}\n\n"
				 "\\begin{enumerate}\n" LIST_ITEMS_SPACING "\n",
				 repo->name.val);
	
	for (size_t n_c = 0; n_c < repo->history->n_authored; n_c++) {
		fprintf(out, "\t\\item \\label{%s:item:%s} ",
				repo->name.val,
				authored[n_c]->hash.val);
		if (settings->print_msg) {
			print_commit_message(out, authored[n_c]);
		}
		fprintf(out, "\\href{%s}{%s} (%s) ",
				repo->format.commit_url(repo->url, authored[n_c]->hash).val,
				authored[n_c]->hash.val,
				format_date(authored[n_c]->date, settings->date_only).val);
		if (settings->show_diffs) {
			print_commit_diffs(out, authored[n_c]);
		}
	}

	fprintf(out, "\\end{enumerate}\n");

co_authored:
	
	if (repo->history->n_co_authored == 0) { return; }

	fprintf(out, "\n\\turtexpar{Co-authored}\n\\label{par:%s-co-authored}\n\n"
				 "\\begin{enumerate}\n" LIST_ITEMS_SPACING "\n",
				 repo->name.val);
	
	for (size_t n_c = 0; n_c < repo->history->n_co_authored; n_c++) {
		fprintf(out, "\t\\item \\label{%s:item:%s} ",
				repo->name.val,
				authored[n_c]->hash.val);
		if (settings->print_msg) {
			print_commit_message(out, co_authored[n_c]);
		}
		fprintf(out, "\\href{%s}{%s} (%s) ",
				repo->format.commit_url(repo->url, authored[n_c]->hash).val,
				co_authored[n_c]->hash.val,
				format_date(co_authored[n_c]->date, settings->date_only).val);
		if (settings->show_diffs) {
			print_commit_diffs(out, authored[n_c]);
		}
	}

	fprintf(out, "\\end{enumerate}\n");
}

static void generate_latex_file_list(FILE *out, const
									 repository_t *repo,
									 const indexes_t *indexes,
									 const settings_t *settings)
{
	commit_t **const authored = indexes->authored;
	commit_t **const co_authored = indexes->co_authored;

	for (size_t n_c = 0; n_c < repo->history->n_authored; n_c++) {
		fprintf(out, "\t\\item \\label{%s:item:%s}",
				repo->name.val,
				authored[n_c]->hash.val);
		if (settings->print_msg) {
			print_commit_message(out, authored[n_c]);
		}
		fprintf(out, "%s: [A] \\href{%s}{%s} %s\n",
				repo->name.val,
				repo->format.commit_url(repo->url, authored[n_c]->hash).val,
				authored[n_c]->hash.val,
				format_date(authored[n_c]->date, settings->date_only).val);
		if (settings->show_diffs) {
			print_commit_diffs(out, authored[n_c]);
		}
	}

	for (size_t n_c = 0; n_c < repo->history->n_co_authored; n_c++) {
		fprintf(out, "\t\\item \\label{%s:item:%s} ",
				repo->name.val,
				authored[n_c]->hash.val);
		if (settings->print_msg) {
			print_commit_message(out, co_authored[n_c]);
		}
		fprintf(out, "%s: [C] \\href{%s}{%s} %s\n",
				repo->name.val,
				repo->format.commit_url(repo->url, authored[n_c]->hash).val,
				co_authored[n_c]->hash.val,
				format_date(co_authored[n_c]->date, settings->date_only).val);
		if (settings->show_diffs) {
			print_commit_diffs(out, authored[n_c]);
		}
	}
}

void generate_latex_file(FILE *out, const repository_array_t *repos, const settings_t *settings)
{
	fprintf(out, "%% This file is automatically generated by TUR.\n"
				 "%% This file is not standalone, you have to "
				 "include it in a LaTeX document with both "
				 "*xcolor* and *hyperref* packages.\n\n"
				 "%% Commands definition\n"
				 "\\newcommand{\\turtexpar}[1]{\\textbf{#1}}");

	if (str_not_empty(settings->title)) {
		fprintf(out, "\n\n\\section{%s}", settings->title.val);
	}

	if (!settings->grouped) {
		fprintf(out, "\n\n\\begin{enumerate}\n" LIST_ITEMS_SPACING "\n");
	}

	for (size_t i = 0; i < repos->count; i++) {
		const repository_t repo = repos->repositories[i];

		if (settings->grouped) {
			generate_latex_file_grouped(out, &repo, &repo.history->indexes, settings);
		} else {
			generate_latex_file_list(out, &repo, &repo.history->indexes, settings);
		}
	}

	if (!settings->grouped) {
		fprintf(out, "\\end{enumerate}\n");
	}
}
