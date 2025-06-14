/* str.c
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

#include "log.h"
#include "str.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY_STR (str_t) { .val="", .len=0 }

str_t empty_str(void)
{
	return str_init(EMPTY_STR.val, EMPTY_STR.len);
}

str_t str_init(const char *str, uint16_t len)
{
	char *copy = malloc(len + 1);
	if (!copy) {
		(void)log_err("str_init: memory allocation failed\n");
		return empty_str();
	}
	if (len > 0) {
		strncpy(copy, str, len);
	}
	copy[len] = '\0';

	return (str_t) {
		.val = copy,
		.len = len
	};
}

inline bool str_is_empty(str_t str)
{
	return str.len == 0;
}

inline bool str_not_empty(str_t str)
{
	return !str_is_empty(str);
}

bool str_equals(str_t str1, str_t str2)
{
	if (str1.len != str2.len) { return false; }
	return strncmp(str1.val, str2.val, str1.len) == 0;
}

bool str_arr_equals(str_t str, const char *expected)
{
	if (str.len != strlen(expected)) return false;
	return strncmp(str.val, expected, str.len) == 0;
}

char get_char(str_t str, uint16_t i)
{
	if (i >= str.len) {
		(void)log_err("get_char: trying to get char with index %d, but the string has length %d\n",
					  i, str.len);
		exit(1);
	}

	return str.val[i];
}

str_t str_concat(str_t str1, str_t str2)
{
	uint16_t new_len = str1.len + str2.len;
	char *new_val = malloc(new_len + 1);
	if (!new_val) {
		(void)log_err("str_concat: memory allocation failed. Trying to concatenate:\n"
					  "    \"%s\"\n"
					  "    \"%s\"\n",
					  str1.val, str2.val);
		return empty_str();
	}
	memcpy(new_val, str1.val, str1.len);
	memcpy(new_val + str1.len, str2.val, str2.len);
	new_val[new_len] = '\0';

	return (str_t) {
		.val = new_val,
		.len = new_len
	};
}

str_t str_copy(str_t source)
{
	return str_init(source.val, source.len);
}

bool str_contains(str_t str, str_t substr)
{
	return strstr(str.val, substr.val) != NULL;
}

bool str_contains_chars(str_t str, const char * substr)
{
	return strstr(str.val, substr) != NULL;
}

bool chars_contains_chars(const char *str, const char * substr)
{
	return strstr(str, substr) != NULL;
}

void str_free(str_t str)
{
	free((void *)str.val);
}

void str_array_free(str_array_t *arr)
{
	for (size_t i = 0; i < arr->len; i++) {
		str_free(arr->strings[i]);
	}
	free(arr->strings);
}
