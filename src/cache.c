/* cache.c
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

#include "cache.h"
#include "commit.h"
#include "codes.h"
#include "log.h"
#include "lookup_table.h"
#include "utils.h"

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FAVORITE_STR "[*]"

/* Just return the key as it is */
uint32_t id_hash(void *key)
{
	return *(uint32_t *)key;
}

static void print_commit_line(FILE *fp, const commit_t *commit)
{
	fprintf(fp, "%s\t%s\n", commit->hash.val, get_first_line(commit->msg).val);
}

static return_code_t parse_commit_file(table_t *repo_table)
{
	return_code_t ret = OK;
	FILE *fp = fopen(COMMITS_FILE, "r");
	if (!fp) {
		log_err("parse_commit_file: cannot open file `%s`\n", COMMITS_FILE);
		return COMMIT_FILE_DOES_NOT_EXIST;
	}

	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	unsigned current_repo_id = (unsigned)-1;
	str_array_t *current_commits = NULL;

	while ((read = getline(&line, &len, fp)) != -1) {
		if (read > 0 && line[read - 1] == '\n') {
			line[--read] = '\0';
		}

		if (line[0] == '+') {
			if (current_repo_id >= 0 && current_commits) {
				table_put(repo_table, current_repo_id, current_commits);
				str_array_free(&current_commits);
			}

			/* Parsing the repo id from this line:  "+ <ID>) <NAME>" */
			ret = parse_commit_id(&current_repo_id, line);
			if (ret != OK) { return ret; }

			const size_t strings_size = DEFUALT_STR_ARR_SIZE * sizeof(str_t);
			current_commits = malloc(sizeof(str_array_t));
			current_commits->strings = malloc(strings_size);
			current_commits->len = 0;
			current_commits->capacity = DEFUALT_STR_ARR_SIZE;

		} else if (current_repo_id >= 0 && current_commits) {
			if (strlen(line) == 0) { continue; }

			if (current_commits->len >= current_commits->capacity) {
				current_commits->capacity += DEFUALT_STR_ARR_SIZE;
				current_commits->strings = realloc(current_commits->strings,
					current_commits->capacity * sizeof(str_t));
			}

			char *end_of_hash = strchr(line, '\t');
			if (!end_of_hash) { return COMMITS_FILE_HASH_CORRUPTED; }
			
			size_t hash_len = (size_t)(end_of_hash - line);
			str_t hash = str_init(line, hash_len);
			current_commits->strings[current_commits->len++] = hash;
		}
	}

	if (current_repo_id >= 0 && current_commits) {
		table_put(repo_table, current_repo_id, current_commits);
		str_array_free(&current_commits);
	}

	free(line);
	fclose(fp);

	return ret;
}

static return_code_t repo_index(repository_t *repo, const str_array_t *commits)
{
	size_t authored_count = 0, co_authored_count = 0;
	work_history_t *history = repo->history;

	for (size_t i = 0; i < commits->len; i++) {
		str_t commit_id = commits->strings[i];
		commit_t *c = get_commit_with_id(&history->commit_arr, commit_id);
		if (!c) {
			(void)log_err("repo_index: cannot find commit `%s`\n",
						  commit_id.val);
			return COMMIT_NOT_FOUND;
		}

		if (c->responsability == AUTHORED) {
			history->indexes.authored[authored_count++] = c;
		} else {
			history->indexes.co_authored[co_authored_count++] = c;
		}
	}

	history->n_authored = authored_count;
	history->n_co_authored = co_authored_count;

	return OK;
}

return_code_t check_or_create_tur_dir(void)
{
	struct stat st = { 0 };

	if (stat(TUR_DIR, &st) == -1) {
		if (mkdir(TUR_DIR, 0700) != 0) {
			(void)log_err("Cannot create the directory `%s`\n", TUR_DIR);
			return CANNOT_CREATE_TUR_DIR;
		}
	}

	return OK;
}

bool commit_file_exists(void)
{
	struct stat st = { 0 };
	return stat(COMMITS_FILE, &st) != -1;
}

return_code_t write_repos_on_file(const repository_array_t *repos)
{
	return_code_t ret = OK;
	FILE *fp = NULL;

	ret = check_or_create_tur_dir();
	if (ret != OK) {
		return ret;
	}

	fp = fopen(COMMITS_FILE, "w");
	if (!fp) {
		(void)log_err("Cannot create file `%s`...\n", COMMITS_FILE);
		return CANNOT_CREATE_COMMITS_FILE;
	}

	for (size_t i = 0; i < repos->count; i++) {
		repository_t *repo = repos->repositories + i;
		const work_history_t *history = repo->history;
		commit_t **const authored = history->indexes.authored;
		commit_t **const co_authored = history->indexes.co_authored;

		fprintf(fp, "+ %u) %s\n", repo->id, repo->name.val);
		for (size_t j = 0; j < history->n_authored; j++) {
			print_commit_line(fp, authored[j]);
		}
		for (size_t j = 0; j < history->n_co_authored; j++) {
			print_commit_line(fp, co_authored[j]);
		}
	}

	fclose(fp);
	return ret;
}

return_code_t rebuild_indexes(const repository_array_t *repos)
{
	return_code_t ret = OK;
	table_t repo_table;

	if (!commit_file_exists()) {
		(void)log_err("rebuild_indexes: commit file does not exist\n");
		return COMMIT_FILE_DOES_NOT_EXIST;
	}

	table_init(&repo_table, 10, 10, id_hash);
	ret = parse_commit_file(&repo_table);
	if (ret != OK) { return ret; }

	for (size_t i = 0; i < repos->count; i++) {
		str_array_t *commits = table_get(&repo_table, i);
		if (!commits) { continue; }

		ret = repo_index((repos->repositories + i), commits);
		if (ret != OK) { return ret; }
	}

	return ret;
}

return_code_t delete_cache(void)
{
	if (remove(TUR_DIR) != 0) { return CANNOT_DELETE_TUR_DIR; }
	return OK;
}

return_code_t delete_commits_file(void)
{
	if (remove(COMMITS_FILE) != 0) { return CANNOT_DELETE_COMMITS_FILE; }
	return OK;
}
