/* commit.h
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

#ifndef __COMMIT_H__
#define __COMMIT_H__

#include "array.h"
#include "settings.h"
#include "str.h"

#include <stdint.h>
#include <time.h>

#define GIT_HASH_LEN 40

typedef array_t  commit_arr_t;

typedef enum {
	AUTHORED,
	CO_AUTHORED
} responsability_t;

typedef struct {
	size_t files_changed;
	size_t lines_added;
	size_t lines_removed;
} commit_stats_t;

typedef struct {
	str_t hash;
	responsability_t responsability;
	time_t date;
	str_t msg;
	commit_stats_t stats;
} commit_t;

/* Commits in the indexes are just pointers to the commits in commit_arr.
 * !!! DO NOT FREE THEM !!!
 */
typedef struct {
	commit_t **authored;
	commit_t **co_authored;
} indexes_t;

typedef struct {
	commit_arr_t *commit_arr;
	size_t n_authored;
	size_t n_co_authored;
	indexes_t indexes;
	size_t tot_lines_added;
	size_t tot_lines_removed;
} work_history_t;

work_history_t *get_commit_history(str_t repo_path, const char *branch_name, const settings_t *settings);
commit_t *get_commit_with_id(const commit_arr_t* commit_arr, str_t id);
commit_t *commit_copy(const commit_t *source);
work_history_t *history_copy(const work_history_t *src);
void commit_free(commit_t *commit);
void history_free(work_history_t **history);

/*
 * Commit arrays
 */
void commit_array_init(commit_arr_t **arr);
commit_t *commit_array_get(const commit_arr_t *src, size_t i);
return_code_t commit_array_add(commit_arr_t *src, commit_t *commit);
commit_arr_t *commit_array_copy(const commit_arr_t *src);
bool commit_array_contains(const commit_arr_t *src, commit_t *commit);
void commit_array_free(commit_arr_t **arr);

#endif /* __COMMIT_H__ */
