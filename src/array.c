/* array.c
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

#include "array.h"
#include "codes.h"

#include <stdint.h>
#include <stdlib.h>

#define DEFAULT_ARRAY_SIZE 10

return_code_t array_init(array_t **arr, size_t elem_sz)
{
	array_t *array = malloc(sizeof(array_t));
	if (!array) { return RUNTIME_MALLOC_ERROR; }
	array->len = 0;
	array->capacity = DEFAULT_ARRAY_SIZE;
	array->element_size = elem_sz;
	array->values = malloc(DEFAULT_ARRAY_SIZE * elem_sz);
	if (!array->values) {
		free(array);
		return RUNTIME_ARRAY_REALLOC_ERROR;
	}
	*arr = array;

	return OK;
}

return_code_t array_add(array_t *src, void *elem, assign_fn_t assign_fn)
{
	if (src->len >= src->capacity) {
		src->capacity += DEFAULT_ARRAY_SIZE;
		src->values = realloc(src->values, src->capacity * src->element_size);
		if (!src->values) { return RUNTIME_ARRAY_REALLOC_ERROR; }
	}
	assign_fn((uint8_t *)src->values + src->len * src->element_size,
			  elem);
	src->len++;

	return OK;
}

array_t *array_copy(const array_t *src, assign_fn_t assign_fn)
{
	if (!src) { return NULL; }

	size_t elem_size = src->element_size;
	array_t *copy = NULL;
	array_init(&copy, elem_size);

	for (size_t i = 0; i < src->len; i++) {
		uint8_t *arr_offset = (uint8_t *)src->values + i * elem_size;
		if (array_add(copy, arr_offset, assign_fn) != OK) { return NULL; }
	}

	return copy;
}

bool array_contains(const array_t *src, void *elem, compare_fn_t compare_elem)
{
	for (size_t i = 0; i < src->len; i++) {
		size_t offset = i * src->element_size;
		if (compare_elem((uint8_t *)src->values + offset, elem) == 0) {
			return true;
		}
	}
	return false;
}

void array_free(array_t **arr, free_fn_t free_element_fn)
{
	const array_t *array = *arr;
	for (size_t i = 0; i < array->len; i++) {
		free_element_fn((uint8_t *)array->values + i * array->element_size);
	}
	free(array->values);
	free(*arr);
	*arr = NULL;
}
