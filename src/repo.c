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
#include "repo.h"
#include "settings.h"
#include "str.h"

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

static repository_t init_repo(str_t path, str_t url)
{
	return (repository_t) {
		.path = path,
		.url = url,
		.name = get_repo_name(path),
		.history = NULL
	};
}

repository_t parse_repository(const char *line, ssize_t len)
{
	repository_t repo = { 0 };
	const char *bracket_open = strchr(line, '[');
	
	if (bracket_open == NULL) {
		return init_repo(str_init(line, len), empty_str());
	}
	
	size_t path_len = bracket_open - line;
	repo.path = str_init(line, path_len);
	repo.name = get_repo_name(repo.path);

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
		url_len = len - path_len - 1;
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
	
	return repo;
}

return_code_t get_repos_array(settings_t settings, repository_array_t *repos)
{
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
			size_t new_capacity = repos->capacity + DEFAULT_N_REPOSITORY;
			repository_t *new_repos = realloc(repos->repositories, new_capacity * sizeof(repository_t));
			if (!new_repos) {
				fprintf(stderr, "[get_repos_array] memory allocation failed while expanding repository array\n");
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
