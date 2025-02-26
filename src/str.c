/* str.c
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

#include "str.h"

#include <stdint.h>
#include <string.h>

str_t str_init(const char *str, uint16_t len)
{
	return (str_t) {
		.val = str,
		.len = len
	};
}

/* this should be used only if it's guaranteed that str is NULL terminating */
str_t str_init_unbounded(const char *str)
{
	return (str_t) {
		.val = str,
		.len = strnlen(str, UINT16_MAX)
	};
}
