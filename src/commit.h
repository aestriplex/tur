/* commit.h
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

#ifndef __COMMIT_H__
#define __COMMIT_H__

#include "repo.h"
#include "settings.h"
#include "str.h"

#include <stdint.h>

#define COAUTHOR_PREFIX "Co-authored-by:"
#define COAUTHOR_PREFIX_LEN 15
#define GIT_HASH_LEN 40

typedef enum {
	AUTHORED,
	CO_AUTHORED
} responsability_t;

typedef struct {
	str_t hash;
	responsability_t responsability;
	str_t date;
} commit_t;

typedef struct _commit_history {
	commit_t commit;
	struct _commit_history *parent;
} commit_history_t;

commit_history_t *get_commit_history(repository_t repo, settings_t settings);

#endif /* __COMMIT_H__ */
