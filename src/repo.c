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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>

static void print_commit_info(const git_commit *commit) {
	const git_signature *author = git_commit_author(commit);
	printf("Commit: %s\n", git_oid_tostr_s(git_commit_id(commit)));
	printf("Author: %s <%s>\n", author->name, author->email);
	printf("Date: %s", ctime((const time_t *) &author->when.time));
	printf("Message: %s\n\n", git_commit_message(commit));
}

static repository_t parse_repository(const char *line, ssize_t len)
{
	repository_t repo = { 0 };
	
	const char *bracket_open = strchr(line, '[');
	if (bracket_open == NULL) {
		repo.path = str_init(line, len);
		repo.url = str_init_unbounded(0);
		goto ret;
	}
	
	const char *bracket_close = strchr(bracket_open, ']');
	if (bracket_close == NULL) {
		size_t path_len = bracket_open - line;
		repo.path = str_init(line, path_len);
		
		size_t url_len = len - path_len - 1;
		repo.url = str_init(bracket_open + 1, url_len);
		goto ret;
	}
	
	size_t path_len = bracket_open - line;
	size_t url_len = bracket_close - bracket_open - 1;
	
	repo.path = str_init(line, path_len);
	repo.url = str_init(bracket_open + 1, url_len);

ret:
	return repo;
}

return_code_t get_repos_list(settings_t settings, repository_list_t *list)
{
	size_t len = 0;
	ssize_t read;
	char * line = NULL;
	repository_list_t *current = list;
	FILE *repos_list = fopen(settings.repos_path.val, "r");

	if (!repos_list) { return INVALID_REPOS_LIST_PATH; }

	while ((read = getline(&line, &len, repos_list)) != -1) {
		current->repository = parse_repository(line, read);
		current->next = malloc(sizeof(repository_list_t));
		if (!current->next) {
			fprintf(stderr, "Cannot allocate memory for repository `%s`\n", line);
			continue;
		}
		current = current->next;
	}
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
		fprintf(stderr, "Failed to open repository\n");
		goto ret;
	}

	if (git_revwalk_new(&walker, git_repo) != 0) {
		fprintf(stderr, "Failed to create revwalk\n");
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
