/* commit.c
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
#include "str.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <git2.h>

#define COAUTHOR_PREFIX "Co-authored-by:"
#define COAUTHOR_PREFIX_LEN 15
#define COMMIT_ARRAY_DEFAULT_SIZE 10

static bool is_author(const git_signature *author, str_t *emails, int n_emails)
{
	for (int i = 0; i < n_emails; i++) {
		if (str_arr_equals(emails[i], author->email)) { return true; }
	}
	return false;
}

static bool is_co_author(const char *message, str_t *emails, int n_emails)
{
	char *line = (char *)message;

	while (line) {
		const char *next_line = strchr(line, '\n');
		size_t line_len = (next_line) ? (size_t)(next_line - line) : strlen(line);

		if (strncmp(line, COAUTHOR_PREFIX, COAUTHOR_PREFIX_LEN) == 0) {
			const char *coauthor_info = line + COAUTHOR_PREFIX_LEN;

			while (*coauthor_info == ' ' && (size_t)(coauthor_info - line) < line_len) {
				coauthor_info++;
			}

			const char *email_start = strchr(coauthor_info, '<');
			const char *email_end = strchr(coauthor_info, '>');

			if (email_start && email_end && email_start < email_end) {
				size_t email_len = email_end - email_start - 1;
				char *coauthor_email = malloc(email_len + 1);
				if (coauthor_email) {
					strncpy(coauthor_email, email_start + 1, email_len);
					coauthor_email[email_len] = '\0';

					for (int i = 0; i < n_emails; i++) {
						if (str_arr_equals(emails[i], coauthor_email)) {
							free(coauthor_email);
							return true;
						}
					}
					free(coauthor_email);
				}
			}
		}
		line = (next_line) ? (char *)next_line + 1 : NULL;
	}

	return false;
}

static inline bool is_merge_commit(const char*message)
{
	return chars_contains_chars(message, "Merge")
	       || chars_contains_chars(message, "merge");
}

static void print_error(uint16_t return_code, const char *hash)
{
	switch (return_code) {
	case PARENT_COMMIT_UNAVAILBLE:
		(void)log_err("cannot retrieve parent for commit %s. ERROR: %d\n", hash, return_code);
		break;
	case COMPARE_TREES_ERROR:
		(void)log_err("cannot compare trees for commit %s and its parent. ERROR: %d\n",
				hash, return_code);
		break;
	case CANNOT_RETRIEVE_STATS:
		(void)log_err("cannot get stats for commit %s. ERROR: %d\n", hash, return_code);
		break;
	default:
		(void)log_err("an unknown error occurred while traversing commit "
					  "tree on commit %s. ERROR: %d\n", hash, return_code);
		break;
	}
}

static uint16_t get_commit_stats(commit_stats_t *stats, const git_commit *commit, const git_repository *repo)
{
	int res;
	
	if (git_commit_parentcount(commit) == 0) { return OK; }
	
	git_commit *parent_commit = NULL;
	res = git_commit_parent(&parent_commit, commit, 0);
	if (res != 0) { return PARENT_COMMIT_UNAVAILBLE; }
	
	git_tree *commit_tree = NULL, *parent_tree = NULL;
	git_commit_tree(&commit_tree, commit);
	git_commit_tree(&parent_tree, parent_commit);
	
	git_diff *diff = NULL;
	res = git_diff_tree_to_tree(&diff, (git_repository *)repo, parent_tree, commit_tree, NULL); 
	if (res != 0) { return COMPARE_TREES_ERROR; }
	
	git_diff_stats *git_stats = NULL;
	res = git_diff_get_stats(&git_stats, diff);
	if (res != 0) { return CANNOT_RETRIEVE_STATS; }

	*stats = (commit_stats_t) {
		.files_changed = git_diff_stats_files_changed(git_stats),
		.lines_added = git_diff_stats_insertions(git_stats),
		.lines_removed = git_diff_stats_deletions(git_stats)
	};

	return OK;
}

work_history_t *get_commit_history(str_t repo_path, const char *branch_name, const settings_t *settings)
{
	git_repository *git_repo = NULL;
	git_revwalk *walker = NULL;
	git_commit *raw_commit = NULL;
	git_reference *branch_ref = NULL;
	git_object *branch_commit = NULL;
	work_history_t *history = NULL;
	size_t n_authored = 0, n_co_authored = 0;
	git_oid oid;

	if (git_repository_open(&git_repo, repo_path.val) != 0) {
		(void)log_err("Failed to open repository `%s`\n", repo_path.val);
		goto ret;
	}

	if (git_revwalk_new(&walker, git_repo) != 0) {
		(void)log_err("An error occurred while reading from `%s`\n", repo_path.val);
		goto cleanup;
	}

	if (branch_name) {
		if (git_branch_lookup(&branch_ref, git_repo, branch_name, GIT_BRANCH_LOCAL) != 0) {
			(void)log_err("%s: Cannot find a *local* branch named `%s`\n", repo_path.val, branch_name);
			git_revwalk_free(walker);
			goto cleanup;
		}

		if (git_reference_peel(&branch_commit, branch_ref, GIT_OBJECT_COMMIT) != 0) {
			(void)log_err("%s: Cannot find the HEAD of `%s`\n", repo_path.val, branch_name);
			git_revwalk_free(walker);
			git_reference_free(branch_ref);
			goto cleanup;
		}
	}

	git_revwalk_sorting(walker, GIT_SORT_TIME);

	if (!branch_name) {
		git_revwalk_push_head(walker);
	} else {
		if (git_revwalk_push(walker, git_object_id(branch_commit)) != 0) {
			log_err("Cannot push the initial commit\n");
			goto cleanup;
		}
	}

	history = malloc(sizeof(work_history_t));
	history->commit_arr = (commit_arr_t) {
		.capacity = COMMIT_ARRAY_DEFAULT_SIZE,
		.count = 0,
		.commits = malloc(COMMIT_ARRAY_DEFAULT_SIZE * sizeof(commit_t))
	};
	history->tot_lines_added = 0;
	history->tot_lines_removed = 0;
	
	if (!history->commit_arr.commits) {
		(void)log_err("get_commit_history: cannot allocate commits array. "
					  "Capacity: %zu; count: %zu\n",
					  history->commit_arr.capacity, history->commit_arr.count);
		goto cleanup;
	}

	responsability_t res;

	while (git_revwalk_next(&oid, walker) == 0) {

		if (git_commit_lookup(&raw_commit, git_repo, &oid) != 0) { continue; }
		
		const char *hash = git_oid_tostr_s(git_commit_id(raw_commit));
		const char *msg = git_commit_message(raw_commit);
		const git_signature *author = git_commit_author(raw_commit);

		if (!author || !msg) { goto free_commit; }

		if (settings->no_merge && is_merge_commit(msg)) { goto free_commit; }

		if (is_author(author, settings->emails, settings->n_emails)) {
			res = AUTHORED;
			n_authored++;
		} else if (is_co_author(msg, settings->emails, settings->n_emails)) {
			res = CO_AUTHORED;
			n_co_authored++;
		} else {
			goto free_commit;
		}

		if (history->commit_arr.count >= history->commit_arr.capacity) {
			size_t new_capacity = history->commit_arr.capacity + COMMIT_ARRAY_DEFAULT_SIZE;
			commit_t *new_commits = realloc(history->commit_arr.commits, new_capacity * sizeof(commit_t));
			if (!new_commits) {
				(void)log_err("get_commit_history: memory allocation failed while expanding repository array\n");
				goto cleanup;
			}
			history->commit_arr.commits = new_commits;
			history->commit_arr.capacity = new_capacity;
		}

		commit_stats_t stats = { 0 };
		const uint16_t return_code = get_commit_stats(&stats, raw_commit, git_repo);
		if (return_code != OK) {
			print_error(return_code, hash);
			return NULL;
		}
		const size_t index = n_authored + n_co_authored - 1;
		history->commit_arr.commits[index] = (commit_t) {
			.hash = str_init(hash, GIT_HASH_LEN),
			.date = (time_t) author->when.time,
			.msg = str_init(msg, (uint16_t)strlen(msg)),
			.responsability = res,
			.stats = stats
		};
		history->commit_arr.count++;
		history->tot_lines_added += stats.lines_added;
		history->tot_lines_removed += stats.lines_removed;

	free_commit:
		git_commit_free(raw_commit);
	}

	git_revwalk_free(walker);
	git_object_free(branch_commit);
	git_reference_free(branch_ref);

	history->n_authored = n_authored;
	history->n_co_authored = n_co_authored;

	/* Indexes are built in walk procedure (see walk.c) */
	history->indexes = (indexes_t) {
		.authored = NULL,
		.co_authored = NULL
	};

cleanup:
	git_repository_free(git_repo);
ret:
	return history;
}
