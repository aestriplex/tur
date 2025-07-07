/* utils.h
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

#ifndef __UTILS_H__
#define __UTILS_H__

#include "codes.h"
#include "settings.h"
#include "str.h"

#include <time.h>

#define RESET  "\033[0m"
#define RED    "\033[31m"
#define GREEN  "\033[32m"
#define YELLOW "\033[33m"
#define TICK   "\u2713"
#define CROSS  "\u2A09"

#define ASCII_SPACE       32
#define DATE_PATTERN      "%b %d, %Y"
#define DATE_PATTERN_SIZE 13 /* Mar 10, 2025 */

str_t format_date(time_t timestamp, bool date_only);
str_t get_github_commit_url(str_t repo_url, str_t commit_hash);
str_t get_gitlab_commit_url(str_t repo_url, str_t commit_hash);
str_t get_raw_url(str_t repo_url, str_t commit_hash);
str_t get_first_line(str_t input);
char* trim_whitespace(const char *str);
str_t escape_special_chars(str_t input);
str_t get_editor_or_default(void);
return_code_t parse_commit_id(unsigned *id, const char *line);

#endif /* __UTILS_H__ */
