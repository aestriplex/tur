/* editor.c
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

#include "log.h"
#include "repo.h"
#include "settings.h"
#include "str.h"
#include "utils.h"

#include <stdlib.h>
#include <unistd.h>

#define COMMITS_FILE ".tur/commits"

static void print_commit_line(FILE *fp, const commit_t *commit)
{
	fprintf(fp, "%s\t%s\n", commit->hash.val, get_first_line(commit->msg).val);
}

return_code_t write_repos_on_file(const repository_array_t *repos,
								  const settings_t *settings)
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

return_code_t choose_commits_through_editor(const settings_t *settings)
{
	return_code_t ret = OK;

	pid_t pid = fork();

	if (pid < 0) { return CANNOT_FORK_PROCESS; }

	if (pid == 0) {
		char *args[] = { (char *)settings->editor.val, COMMITS_FILE, NULL };
		execvp(settings->editor.val, args);
		return EXTERNAL_EDITOR_FAILED;
	}

	int status;
	waitpid(pid, &status, 0);

	return ret;
}
