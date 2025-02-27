/* commit.c
 * -----------------------------------------------------------------------
 * Copyright (C) 2025  Matteo Nicoli
 *
 * This file is part of tur
 *
 * tur is free software; you can redistribute it and/or modify
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
#include "str.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <git2.h>

static inline bool is_author(const git_signature *author, str_t email)
{
	return str_arr_equals(email, author->email);
}

static bool is_co_author(const git_commit *commit, str_t email)
{
	const char *message;
	char *line;
	
	message = git_commit_message(commit);
	if (!message) {
		fprintf(stderr, "Cannot get message for commit %s\n",
				git_oid_tostr_s(git_commit_id(commit)));
		return false;
	}

	line = message;
	while (line) {
		const char *next_line = strchr(line, '\n');
		size_t line_len = (next_line) ? (size_t)(next_line - line) : strlen(line);

		if (strncmp(line, COAUTHOR_PREFIX, COAUTHOR_PREFIX_LEN) == 0) {
			const char *coauthor_info = line + COAUTHOR_PREFIX_LEN;

			while (*coauthor_info == ' ' && (coauthor_info - line) < line_len) {
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

					bool is_coauthor = str_arr_equals(email, coauthor_email);
					free(coauthor_email);

					if (is_coauthor) { return true; }
				}
			}
		}
		line = (next_line) ? next_line + 1 : NULL;
	}

	return false;
}

commit_history_t *get_commit_history(repository_t repo, settings_t settings)
{
	commit_history_t *history = NULL, *current = NULL;
	git_repository *git_repo = NULL;
	git_revwalk *walker = NULL;
	git_commit *raw_commit = NULL;
	git_oid oid;

	git_libgit2_init();

	if (git_repository_open(&git_repo, repo.path.val) != 0) {
		fprintf(stderr, "Failed to open repository `%s`\n", repo.path.val);
		goto ret;
	}

	if (git_revwalk_new(&walker, git_repo) != 0) {
		fprintf(stderr, "An error occurred while reading from `%s`\n", repo.path.val);
		goto cleanup;
	}

	git_revwalk_push_head(walker);
	git_revwalk_sorting(walker, GIT_SORT_TIME);

	history = malloc(sizeof(commit_history_t));
	current = history;

	while (git_revwalk_next(&oid, walker) == 0) {

		if (git_commit_lookup(&raw_commit, git_repo, &oid) != 0) { continue; }
		
		const char *hash = git_oid_tostr_s(git_commit_id(raw_commit));
		const git_signature *author = git_commit_author(raw_commit);
		if (!author) { continue; }
		const char *date = ctime((const time_t *) &author->when.time);
		responsability_t res;

		if (is_author(author, settings.email)) {
			res = AUTHORED;
		} else if (is_co_author(raw_commit, settings.email)) {
			res = CO_AUTHORED;
		} else {
			goto free_commit;
		}
		current->parent = malloc(sizeof(commit_history_t));
		current = current->parent;
		current->commit = (commit_t) {
			.hash = str_init(hash, GIT_HASH_LEN),
			.date = str_init(date, (uint16_t) strlen(date)),
			.responsability = res,
		};
		current->parent = NULL;
		

free_commit:
		git_commit_free(raw_commit);
	}

	git_revwalk_free(walker);
	git_libgit2_shutdown();

	current->parent = NULL;

cleanup:
	git_repository_free(git_repo);
ret:
	return history;
}
