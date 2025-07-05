/* lookup_table.h
 * ------------------------------------------------------------------------
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

#ifndef __LOOKUP_TABLE_H__
#define __LOOKUP_TABLE_H__

#include "str.h"

#include <stdlib.h>
#include <stdint.h>

/* 100 repositories should be enough for most scenarios */
#define DEFAULT_MAP_SIZE     100
/* 100 commit per repository should be enough as a starting value */
#define DEFUALT_STR_ARR_SIZE 100

typedef enum {
	LM_OK                              = 0x00,
	LM_CANNOT_INIT_TABLE               = 0x01,
	LM_CANNOT_INIT_TABLE_PAIRS         = 0x02,
	LM_CANNOT_INSERT_VALUE             = 0x03,
	LM_CANNOT_REMOVE_NONEXISTENT_VALUE = 0x04,
	LM_NULL_VALUE_NOT_ALLOWED          = 0x05,
	LM_VALUE_NOT_FOUND                 = 0x06,
	LM_MAP_SIZE_OVERFLOW               = 0x07,
	LM_FAILED_EXPANDING_MAP_SIZE       = 0x08,
	LM_NULL_KEY_ARRAY                  = 0x09,
	LM_CANNOT_RESIZE_STRING_ARRAY      = 0x10,

	LM_RUNTIME_ARROR                   = 0XFF,
} table_status_t;

typedef uint32_t (*hash_fn_t)(void *);

typedef struct {
	uint32_t key;
	str_array_t *value;
} pair_t;

typedef struct {
	pair_t *pairs;
	size_t max_size;
	size_t increment;
    size_t size;
	hash_fn_t hash;
} table_t;

table_status_t table_init(table_t *table, size_t capacity, size_t increment,
						  hash_fn_t hash);
table_status_t table_put(table_t *table, uint32_t key, const str_array_t *val);
table_status_t table_add(table_t *table, uint32_t key, const str_t val);
table_status_t table_remove(table_t * table, uint32_t key);
str_array_t *table_get(const table_t * table, uint32_t key);
table_status_t table_get_pairs(const table_t *table, pair_t *keys);
void table_free(table_t *table);

#endif /* __LOOKUP_TABLE_H__ */
