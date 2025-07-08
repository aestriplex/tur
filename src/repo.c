/* repo.c
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
#include "log.h"
#include "repo.h"
#include "settings.h"
#include "str.h"
#include "utils.h"

#include <ctype.h>
#include <libgen.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static str_t get_repo_name(str_t repo_path)
{
	char *repo_name = basename((char *)repo_path.val);
	return str_init(repo_name, (uint16_t) strlen(repo_name));
}

static repository_t init_repo(str_t path, unsigned id)
{
	return (repository_t) {
		.id = id,
		.path = path,
		.url = empty_str(),
		.name = get_repo_name(path),
		.history = NULL,
		.format = { 0 },
	};
}

static fmt_commit_url select_function(str_t url)
{
	if (str_contains_chars(url, "github.com")) { return &get_github_commit_url; }
	if (str_contains_chars(url, "gitlab")) { return &get_gitlab_commit_url; }
	return &get_raw_url;
}

static str_array_t *get_branches(const char *line, size_t len)
{
	str_array_t *result = NULL;

	char *to_parse = malloc(len + 1);
	if (!to_parse) {
		(void)log_err("get_branches: cannot allocate to_parse string\n");
		exit(1);
	}

	memcpy(to_parse, line, len);
	to_parse[len] = '\0';

	size_t n_branches = 1;
	for (char *p = to_parse; *p; ++p) {
		if (*p == ',') n_branches++;
	}

	str_array_init(&result);

	char *token = strtok(to_parse, ",");
	while (token) {
		str_t branch_str = str_init(token, strnlen(token, len));
		if (str_array_add(result, branch_str) != OK) {
			(void)log_err("get_branches: an error occurred while adding a "
						  "branch in branches array...");
			str_array_free(&result);
			free(to_parse);
			return NULL;
		}
		token = strtok(NULL, ",");
	}

	free(to_parse);
	return result;
}

repository_t parse_repository(const char *line, ssize_t len, unsigned id)
{
	repository_t repo = { 0 };
	const char *bracket_open = strchr(line, '[');
	
	if (bracket_open == NULL) {
		return init_repo(str_init(line, len), id);
	}

	size_t path_and_banches_len = bracket_open - line;
	const char *path_str = line;
	const char *branches_str = NULL;
	size_t path_len = 0;
	size_t branches_len = 0;
	const char *colon = memchr(line, ':', path_and_banches_len);

	if (colon) {
		path_len = (size_t)(colon - line);
		branches_str = colon + 1;
		branches_len = path_and_banches_len - path_len - 1;
	} else {
		path_len = path_and_banches_len;
		branches_str = NULL;
		branches_len = 0;
	}

	repo.id = id;
	repo.path = str_init(path_str, path_len);
	repo.name = get_repo_name(repo.path);
	repo.branches = branches_str
					? get_branches(branches_str, branches_len)
					: NULL;

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
	
	size_t url_len;
	if (!bracket_close) {
		url_len = len - path_and_banches_len - 1;
	} else {
		url_len = bracket_close - bracket_open - 1;
	}

	repo.url = url_len == 0
			   ? empty_str()
			   : str_init(bracket_open + 1, url_len);

	if (repo.url.len != 0 && get_char(repo.url, repo.url.len - 1) != '/') {
		str_t postfix = str_init("/", 1);
		str_t new_url = str_concat(repo.url, postfix);
		str_free(repo.url);
		str_free(postfix);
		repo.url = new_url;
	}
	repo.format = (fmt_t) {
		.commit_url = select_function(repo.url),
	};

	return repo;
}

return_code_t get_repos_array(repository_array_t *repos, const settings_t *settings)
{
	return_code_t ret = OK;
	unsigned id = 0;
	size_t len = 0, max_name_len = 0;
	ssize_t read;
	char *line = NULL;
	FILE *repos_list;

	repos_list = fopen(settings->repos_path.val, "r");
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
			size_t new_capacity = repos->capacity + DEFAULT_N_REPOSITORY;
			repository_t *new_repos = realloc(repos->repositories, new_capacity * sizeof(repository_t));
			if (!new_repos) {
				(void)log_err("get_repos_array: memory allocation failed "
							  "while expanding repository array\n");
				break;
			}
			repos->repositories = new_repos;
			repos->capacity = new_capacity;
		}
		repos->repositories[repos->count] = parse_repository(line, read, id);
		id++;
		repos->count++;
	}

	for (size_t i = 0; i < repos->count; i++) {
		if (repos->repositories[i].name.len > max_name_len) {
			max_name_len = repos->repositories[i].name.len;
		}
	}
	repos->max_name_len = max_name_len;

	ret = log_info("%lu repositories found...\n", repos->count);

	if (line) {
		free(line);
	}
	fclose(repos_list);

	return ret;
}
