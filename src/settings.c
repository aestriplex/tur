/* settings.c
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

#include "settings.h"
#include "str.h"

#include <stdlib.h>

#define DEFAULT_REPOS_LIST_PATH      ".rlist"
#define DEFAULT_REPOS_LIST_PATH_SIZE 6

settings_t default_settings(void)
{
	return (settings_t) {
		.output_mode = STDOUT,
		.output = empty_str(),
		.clear_cache = false,
		.repos_path = str_init(DEFAULT_REPOS_LIST_PATH, DEFAULT_REPOS_LIST_PATH_SIZE),
		.emails = NULL,
		.n_emails = 0,
		.grouped = false,
		.sorted = false,
		.show_diffs = false,
		.print_msg = false,
	};
}
