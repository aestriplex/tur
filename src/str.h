/* str.h
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

#ifndef __STR_H__
#define __STR_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
	const char*val;
	uint16_t len;
} str_t;

str_t str_init(const char *str, uint16_t len);
str_t empty_str(void);
bool str_equals(str_t str1, str_t str2);
bool str_arr_equals(str_t str, const char *expected);
char get_char(str_t str, uint16_t i);
str_t str_concat(str_t str1, str_t str2);
void str_free(str_t str);

#endif /* __STR_H__ */ 
