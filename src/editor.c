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
#include "settings.h"
#include "str.h"
#include "utils.h"

#include <stdlib.h>
#include <unistd.h>

#define COMMITS_FILE ".tur/commits"

return_code_t choose_commits_through_editor(const settings_t *settings)
{
	return_code_t ret = OK;

	ret = check_or_create_tur_dir();
	if (ret != OK) {
		return ret;
	}

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
