/* array.h
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

#ifndef __ARRAY_H__
#define __ARRAY_H__

#include "codes.h"

#include <stdbool.h>
#include <stdlib.h>

typedef void (*assign_fn_t)(void*, void*);
typedef int (*compare_fn_t)(void*, void*);
typedef void (*free_fn_t)(void*);

typedef struct {
	void *values;
	size_t len;
	size_t capacity;
	size_t element_size;
} array_t;

void array_init(array_t **arr, size_t elem_sz);
return_code_t array_add(array_t *src, void *elem, assign_fn_t assign_fn);
array_t *array_copy(const array_t *src, assign_fn_t assign_fn);
bool array_contains(const array_t *src, void *elem, compare_fn_t compare_elem);
void array_free(array_t **arr, free_fn_t free_element_fn);

#endif /* __ARRAY_H__ */
