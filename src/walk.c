/* walk.c
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
#include "commit.h"
#include "repo.h"
#include "settings.h"
#include "walk.h"

return_code_t walk_through_repos(const repository_array_t *repos, settings_t settings)
{
	repository_t repo;
	commit_history_t *history;
	for (size_t i = 0; i < repos->count; i++) {
		repo = repos->repositories[i];
		history = get_commit_history(repo, settings);
	}

	printf("%p\n", history);

	return OK;
}
