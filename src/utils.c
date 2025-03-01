/* utils.c
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

#include "commit.h"
#include "repo.h"
#include "str.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#define GITHUB_URL      "commit/"
#define GITHUB_URL_SIZE 7

str_t time_to_string(const time_t *timestamp)
{
	const char *date = ctime(timestamp);
	return str_init(date, (uint16_t) strlen(date));
}

str_t get_github_url(str_t repo_url, str_t commit_hash)
{
	const size_t new_len = repo_url.len + GITHUB_URL_SIZE + GIT_HASH_LEN;
	char *url = malloc(new_len);
	sprintf(url, "%s%s%s", repo_url.val, GITHUB_URL, commit_hash.val);

	return str_init(url, new_len);
}
