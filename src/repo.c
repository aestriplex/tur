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
		str_free(branch_str);
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
	size_t len = 0;
	ssize_t read;
	char *line = NULL;
	FILE *repos_list;

	repos_list = fopen(settings->repos_path.val, "r");
	if (!repos_list) { return INVALID_REPOS_LIST_PATH; }

	while ((read = getline(&line, &len, repos_list)) != -1) {
		char *trimmed = line;
		while (*trimmed && isspace((unsigned char)*trimmed)) { trimmed++; }

		char *end = trimmed + strlen(trimmed) - 1;
		while (end > trimmed && isspace((unsigned char)*end)) {
			*end = '\0';
			end--;
		}

		if (*trimmed == '\0') { continue; } 

		repository_t repo = parse_repository(line, read, id);
		ret = repo_array_add(repos, &repo);
		if (ret != OK) {
			(void)log_err("get_repos_array: cannot create a repository "
						  "list [%d]\n", ret);
			goto cleanup;
		}

		id++;
	}

	ret = log_info("%lu repositories found...\n", repos->len);

cleanup:
	if (line) {
		free(line);
	}
	fclose(repos_list);

	return ret;
}

repository_stats_t get_repos_stats(const repository_array_t *repos)
{
	repository_stats_t stats = { 0 };

	for (size_t i = 0; i < repos->len; i++) {
		repository_t *repo = repo_array_get(repos, i);
		if (repo->name.len > stats.max_name_len) {
			stats.max_name_len = repo->name.len;
		}
	}

	return stats;
}

repository_t *repository_copy(const repository_t *src)
{
	repository_t *new = malloc(sizeof(repository_t));
	new->url = str_copy(src->url);
	new->path = str_copy(src->path);
	new->name = str_copy(src->name);
	new->id = src->id;
	new->format = src->format;
	new->branches = str_array_copy(src->branches);
	/* add deep copy of  history */
	return new;
}

/*
 * Repository arrays
 */
static void assign_repo(void *src, void *elem)
{
	repository_t *repo = (repository_t *)elem;
	*(repository_t *)src = *repository_copy(repo);
}

static int compare_repo(void *r1, void *r2)
{
	repository_t *repo1 = (repository_t *)r1;
	repository_t *repo2 = (repository_t *)r2;
	return repo1->id > repo2->id
		   ? -1
		   : repo1->id != repo2->id;
}

static void free_repo(void *r)
{
	repository_t *repo = (repository_t *)r;
	str_free(repo->url);
	str_free(repo->path);
	str_free(repo->name);
	str_array_free(&repo->branches);
	history_free(&repo->history);
}

void repo_array_init(repository_array_t **arr)
{
	array_init(arr, sizeof(repository_t));
}

repository_t *repo_array_get(const repository_array_t *src, size_t i)
{
	return (repository_t *)src->values + i;
}

return_code_t repo_array_add(repository_array_t *src, repository_t *repo)
{
	return array_add(src, repo, assign_repo);
}

repository_array_t *repo_array_copy(const repository_array_t *src)
{
	return array_copy(src, assign_repo);
}

bool repo_array_contains(const repository_array_t *src, repository_t *repo)
{
	return array_contains(src, repo, compare_repo);
}

void repo_array_free(repository_array_t **arr)
{
	array_free(arr, free_repo);
}
