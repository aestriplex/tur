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

#include "cache.h"
#include "log.h"
#include "repo.h"
#include "settings.h"
#include "str.h"
#include "utils.h"

#include <stdlib.h>
#include <unistd.h>

return_code_t choose_commits_through_editor(const settings_t *settings)
{
	return_code_t ret = OK;

	pid_t pid = fork();

	if (pid < 0) {
		(void)log_err("choose_commits_through_editor: cannot create a child "
					  "process for the text editor...\n");
		return CANNOT_FORK_PROCESS;
	}

	if (pid == 0) {
		char *args[] = { (char *)settings->editor.val, COMMITS_FILE, NULL };
		execvp(settings->editor.val, args);
		(void)log_err("choose_commits_through_editor: cannot open editor "
					  "`%s`. You can configure the environment variable "
					  "'GIT_EDITOR'\n", settings->editor.val);
		return EXTERNAL_EDITOR_FAILED;
	}

	int status;
	waitpid(pid, &status, 0);

	return ret;
}
