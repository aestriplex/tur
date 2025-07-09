/* walk.h
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

#ifndef __WALK_H__
#define __WALK_H__

#include "commit.h"
#include "repo.h"
#include"settings.h"

#include <pthread.h>

/* This structure is aligned to prevent the sanitize=thread to raise
   a useless warning. */
typedef struct {
	repository_t *repo;
	uint16_t ret;
} __attribute__((aligned(64))) thread_worker_t;

typedef struct {
	size_t n_threads;
	size_t n_workers;
	pthread_t *threads;
	thread_worker_t *workers;
	size_t current_worker;
	pthread_mutex_t current_worker_lock;
	const settings_t *settings;
} thread_pool_t;

return_code_t walk_through_repos(const repository_array_t *repos,
								 const settings_t *settings,
								 repository_stats_t stats);

#endif /* __WALK_H__ */
