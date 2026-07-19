/* latex.c
 * -----------------------------------------------------------------------
 * Copyright (C) 2025 - 2026 Matteo Nicoli
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

static unsigned long common_index;

static const char *favorite_tex(const commit_t *commit)
{
    return commit->is_favorite ? " \\turfav{}\\spc{}" : "";
}

static void print_commit_diffs(FILE *out, const commit_t * commit)
{
    fprintf(out, "%zu file%c changed "
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
                                        const work_history_t *history,
                                        const settings_t *settings)
{
    commit_t **const authored = history->authored_idx;
    commit_t **const co_authored = history->co_authored_idx;

    if (repo->history->n_authored == 0 && repo->history->n_co_authored == 0) { return; }

    fprintf(out, "\n\n\\subsection{%s}\n\\label{subsec:%s}\n", repo->name.val, repo->name.val);
        
    if (repo->history->n_authored == 0) { goto co_authored; }

    fprintf(out, "\n\\turtexpar{Authored}\n\\label{par:%s-authored}\n\n"
                 "\\begin{itemize}\n" LIST_ITEMS_SPACING "\n",
                 repo->name.val);
    
    for (size_t n_c = 0; n_c < repo->history->n_authored; n_c++) {
        fprintf(out, "\t\\item[%s(%lu)] \\label{%s:item:%s} ",
                favorite_tex(authored[n_c]),
                n_c + 1,
                repo->name.val,
                authored[n_c]->hash.val);
        if (settings->print_msg) {
            print_commit_message(out, authored[n_c]);
        }
        fprintf(out, "\\href{%s}{%s} (%s) \\\\ \n",
                repo->format.commit_url(repo->url, authored[n_c]->hash).val,
                authored[n_c]->hash.val,
                format_date(authored[n_c]->date, settings->date_only).val);
        if (settings->show_diffs) {
            print_commit_diffs(out, authored[n_c]);
        }
    }

    fprintf(out, "\\end{itemize}\n");

co_authored:
    
    if (repo->history->n_co_authored == 0) { return; }

    fprintf(out, "\n\\turtexpar{Co-authored}\n\\label{par:%s-co-authored}\n\n"
                 "\\begin{itemize}\n" LIST_ITEMS_SPACING "\n",
                 repo->name.val);
    
    for (size_t n_c = 0; n_c < repo->history->n_co_authored; n_c++) {
            fprintf(out, "\t\\item[%s(%lu)] \\label{%s:item:%s} ",
                favorite_tex(co_authored[n_c]),
                n_c + 1,
                repo->name.val,
                co_authored[n_c]->hash.val);
        if (settings->print_msg) {
            print_commit_message(out, co_authored[n_c]);
        }
        fprintf(out, "\\href{%s}{%s} (%s) \\\\ \n",
                repo->format.commit_url(repo->url, co_authored[n_c]->hash).val,
                co_authored[n_c]->hash.val,
                format_date(co_authored[n_c]->date, settings->date_only).val);
        if (settings->show_diffs) {
            print_commit_diffs(out, co_authored[n_c]);
        }
    }

    fprintf(out, "\\end{itemize}\n");
}

static void generate_latex_file_list(FILE *out, const repository_t *repo,
                                     const work_history_t *history,
                                     const settings_t *settings)
{
    commit_t **const authored = history->authored_idx;
    commit_t **const co_authored = history->co_authored_idx;

    for (size_t n_c = 0; n_c < repo->history->n_authored; n_c++) {
        fprintf(out, "\t\\item[%s(%lu)] \\label{%s:item:%s} ",
                favorite_tex(authored[n_c]),
                common_index,
                repo->name.val,
                authored[n_c]->hash.val);
        fprintf(out, "%s: [A] \\href{%s}{%s} %s \\\\ \n",
                repo->name.val,
                repo->format.commit_url(repo->url, authored[n_c]->hash).val,
                authored[n_c]->hash.val,
                format_date(authored[n_c]->date, settings->date_only).val);
        if (settings->print_msg) {
            print_commit_message(out, authored[n_c]);
        }
        if (settings->show_diffs) {
            print_commit_diffs(out, authored[n_c]);
        }
        common_index++;
    }

    for (size_t n_c = 0; n_c < repo->history->n_co_authored; n_c++) {
        fprintf(out, "\t\\item[%s(%lu)] \\label{%s:item:%s} ",
                favorite_tex(co_authored[n_c]),
                common_index,
                repo->name.val,
                co_authored[n_c]->hash.val);
        fprintf(out, "%s: [C] \\href{%s}{%s} %s \\\\ \n",
                repo->name.val,
                repo->format.commit_url(repo->url, co_authored[n_c]->hash).val,
                co_authored[n_c]->hash.val,
                format_date(co_authored[n_c]->date, settings->date_only).val);
        if (settings->print_msg) {
            print_commit_message(out, co_authored[n_c]);
        }
        if (settings->show_diffs) {
            print_commit_diffs(out, co_authored[n_c]);
        }
        common_index++;
    }
}

void generate_latex_file(FILE *out, const repository_array_t *repos, const settings_t *settings)
{
    fprintf(out, "%% This file is automatically generated by TUR.\n"
                 "%% This file is not standalone, you have to "
                 "include it in a LaTeX document with both "
                 "*xcolor* and *hyperref* packages.\n\n"
                 "%% Commands definition\n"
                 "\\definecolor{americanrose}{rgb}{1.0, 0.01, 0.24}\n"
                 "\\newcommand{\\turtexpar}[1]{\\textbf{#1}}\n"
                 "\\newcommand{\\turfav}{ {\\Large \\textcolor{americanrose}{$\\star$}}}\n"
                 "\\newcommand{\\spc}{$\\quad$}");

    if (str_not_empty(settings->title)) {
        fprintf(out, "\n\n\\section{%s}", settings->title.val);
    }

    if (!settings->grouped) {
        fprintf(out, "\n\n\\begin{itemize}\n" LIST_ITEMS_SPACING "\n");
    }

    common_index = 1;

    for (size_t i = 0; i < repos->len; i++) {
        repository_t *repo = repo_array_get(repos, i);

        if (settings->grouped) {
            generate_latex_file_grouped(out, repo, repo->history, settings);
        } else {
            generate_latex_file_list(out, repo, repo->history, settings);
        }
    }

    common_index = 1;

    if (!settings->grouped) {
        fprintf(out, "\\end{itemize}\n"
                     "\\vspace{1cm}\n\n"
                     "\\noindent (\\turfav{} indicates a valuable contribution)\n");
    }
}
