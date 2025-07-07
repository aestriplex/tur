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

#include "array.h"
#include "codes.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
	const char*val;
	uint16_t len;
} str_t;

typedef array_t str_array_t;

/* Strings */
str_t str_init(const char *str, uint16_t len);
str_t empty_str(void);
str_t str_copy(str_t source);
bool str_is_empty(str_t str);
bool str_not_empty(str_t str);
int str_compare(str_t str1, str_t str2);
bool str_equals(str_t str1, str_t str2);
bool str_arr_equals(str_t str, const char *expected);
char get_char(str_t str, uint16_t i);
str_t str_concat(str_t str1, str_t str2);
bool str_contains(str_t str, str_t substr);
bool str_contains_chars(str_t str, const char * substr);
bool chars_contains_chars(const char *str, const char * substr);
void str_free(str_t str);

/* String arrays */
void str_array_init(str_array_t **arr);
str_t str_array_get(const str_array_t *src, size_t i);
return_code_t str_array_add(str_array_t *src, str_t str);
str_array_t *str_array_copy(const str_array_t *src);
bool str_array_contains(const str_array_t *src, str_t str);
void str_array_free(str_array_t **arr);

#endif /* __STR_H__ */ 
