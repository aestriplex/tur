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

#include "array.h"
#include "codes.h"
#include "commit.h"
#include "settings.h"
#include "str.h"

#include <stdint.h>

typedef str_t (*fmt_commit_url) (str_t, str_t);

typedef struct {
	fmt_commit_url commit_url;
} fmt_t;

typedef struct {
	unsigned id;
	str_t url;
	str_t path;
	str_t name;
	str_array_t *branches;
	fmt_t format;
	work_history_t *history;
} repository_t;

typedef struct {
	size_t max_name_len;
} repository_stats_t;

typedef array_t repository_array_t;

repository_stats_t get_repos_stats(const repository_array_t *repos);
repository_t parse_repository(const char *line, ssize_t len, unsigned id);
return_code_t get_repos_array(repository_array_t *repos, const settings_t *settings);
repository_t *repository_copy(const repository_t *src);

/*
 * Repository arrays
 */
void repo_array_init(repository_array_t **arr);
repository_t *repo_array_get(const repository_array_t *src, size_t i);
return_code_t repo_array_add(repository_array_t *src, repository_t *commit);
repository_array_t *repo_array_copy(const repository_array_t *src);
bool repo_array_contains(const repository_array_t *src, repository_t *commit);
void repo_array_free(repository_array_t **arr);

#endif /* __REPO_H__ */
