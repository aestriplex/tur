/* repo.h
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

#ifndef __REPO_H__
#define __REPO_H__

#include "codes.h"
#include "commit.h"
#include "settings.h"
#include "str.h"

#include <stdint.h>

#define DEFAULT_N_REPOSITORY 10

typedef struct {
	str_t url;
	str_t path;
	str_t name;
	work_history_t *history;
} repository_t;

typedef struct {
	repository_t *repositories;
	size_t count;
	size_t capacity;
} repository_array_t;

repository_t parse_repository(const char *line, ssize_t len);
return_code_t get_repos_array(settings_t settings, repository_array_t *repos);

#endif /* __REPO_H__ */
