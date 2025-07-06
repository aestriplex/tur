/* cache.h
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

#ifndef __CACHE_H__
#define __CACHE_H__

#include "repo.h"
#include "settings.h"

#include <stdint.h>

#define TUR_DIR ".tur/"
#define COMMITS_FILE ".tur/commits_index"

bool commit_file_exists(void);
return_code_t delete_cache(void);
return_code_t delete_commits_file(void);
return_code_t check_or_create_tur_dir(void);
return_code_t write_repos_on_file(const repository_array_t *repos);
return_code_t rebuild_indexes(const repository_array_t *repos);

#endif /* __CACHE_H__ */
