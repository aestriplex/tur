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
#include "log.h"
#include "utils.h"

#include <sys/types.h>
#include <sys/stat.h>

static void print_commit_line(FILE *fp, const commit_t *commit)
{
	fprintf(fp, "%s\t%s\n", commit->hash.val, get_first_line(commit->msg).val);
}

return_code_t check_or_create_tur_dir(void)
{
	struct stat st = { 0 };

	if (stat(TUR_DIR, &st) == -1) {
		if (mkdir(TUR_DIR, 700) != 0) {
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
		(void)log_err("Cannot create file ");
		return CANNOT_CREATE_COMMITS_FILE;
	}

	for (size_t i = 0; i < repos->count; i++) {
		/* We must not use the indexes here, because they are not allocated yet.
		   In case of interactive mode, we are building this file before the index */
		const repository_t *repo = repos->repositories + i;
		const work_history_t *history = repo->history;
		commit_t **const authored = history->indexes.authored;
		commit_t **const co_authored = history->indexes.co_authored;

		fprintf(fp, "+ %s\n", repo->name.val);
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
