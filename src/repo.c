/* repo.c
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

#include "codes.h"
#include "repo.h"
#include "settings.h"
#include "str.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <git2.h>

static void print_commit_info(const git_commit *commit) {
	const git_signature *author = git_commit_author(commit);
	printf("Commit: %s\n", git_oid_tostr_s(git_commit_id(commit)));
	printf("Author: %s <%s>\n", author->name, author->email);
	printf("Date: %s", ctime((const time_t *) &author->when.time));
	printf("Message: %s\n\n", git_commit_message(commit));
}

repository_t parse_repository(const char *line, ssize_t len)
{
	repository_t repo = { 0 };
	const char *bracket_open = strchr(line, '[');
	
	if (bracket_open == NULL) {
		repo.path = str_init(line, len);
		repo.url = EMPTY_STR;
		goto ret;
	}
	
	size_t path_len = bracket_open - line;
	repo.path = str_init(line, path_len);
	
	int nesting = 1;
	const char *bracket_close = NULL;
	for (const char *p = bracket_open + 1; p < line + len; p++) {
		if (*p == '[') {
			nesting++;
		} else if (*p == ']') {
			nesting--;
			if (nesting == 0) {
				bracket_close = p;
				break;
			}
		}
	}
	
	if (!bracket_close) {
		size_t url_len = len - path_len - 1;
		repo.url = str_init(bracket_open + 1, url_len);
	} else {
		size_t url_len = bracket_close - bracket_open - 1;
		repo.url = str_init(bracket_open + 1, url_len);
	}
	
ret:
	return repo;
}

return_code_t get_repos_array(settings_t settings, repository_array_t *repos) {
	size_t len = 0;
	ssize_t read;
	char *line = NULL;
	FILE *repos_list;

	repos_list = fopen(settings.repos_path.val, "r");
	if (!repos_list) { return INVALID_REPOS_LIST_PATH; }

	repos->repositories = malloc(DEFAULT_N_REPOSITORY * sizeof(repository_t));
	if (!repos->repositories) {
		fclose(repos_list);
		return ARRAY_RESIZE_ALLOCATION_ERROR;
	}
	repos->count = 0;
	repos->capacity = DEFAULT_N_REPOSITORY;

	while ((read = getline(&line, &len, repos_list)) != -1) {
		char *trimmed = line;
		while (*trimmed && isspace((unsigned char)*trimmed)) { trimmed++; }

		char *end = trimmed + strlen(trimmed) - 1;
		while (end > trimmed && isspace((unsigned char)*end)) {
			*end = '\0';
			end--;
		}

		if (*trimmed == '\0') { continue; } 

		if (repos->count >= repos->capacity) {
			size_t new_capacity = repos->capacity * 2;
			repository_t *new_repos = realloc(repos->repositories, new_capacity * sizeof(repository_t));
			if (!new_repos) {
				fprintf(stderr, "get_repos_array: memory allocation failed while expanding repository array\n");
				break;
			}
			repos->repositories = new_repos;
			repos->capacity = new_capacity;
		}
		repos->repositories[repos->count] = parse_repository(line, read);
		repos->count++;
	}

	free(line);
	fclose(repos_list);

	return OK;
}

commit_history_t *get_commit_history(repository_t repo, settings_t settings) {
	commit_history_t *history = NULL;
	git_repository *git_repo = NULL;
	git_revwalk *walker = NULL;
	git_oid oid;
	git_commit *commit = NULL;

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

	while (git_revwalk_next(&oid, walker) == 0) {
		if (git_commit_lookup(&commit, git_repo, &oid) == 0) {
			print_commit_info(commit);
			git_commit_free(commit);
		}
	}

	git_revwalk_free(walker);
	git_libgit2_shutdown();

cleanup:
	git_repository_free(git_repo);
ret:
	return history;
}
