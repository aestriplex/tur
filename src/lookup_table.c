/* lookup_table.c
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

#include "codes.h"
#include "log.h"
#include "lookup_table.h"

#include <stdio.h>
#include <stdbool.h>

static table_status_t increase_table_size(table_t *table)
{
	size_t old_size = table->max_size,
		   new_size = old_size + table->increment;

	if (new_size <= old_size) { return LM_MAP_SIZE_OVERFLOW; }
	
	table->pairs = realloc(table->pairs, new_size * sizeof(pair_t));
	if (!table->pairs) { return LM_FAILED_EXPANDING_MAP_SIZE; }

	for (size_t i = old_size; i < new_size; i++) {
		table->pairs[i].key = 0;
		table->pairs[i].value = NULL;
	}

	table->max_size = new_size;
	return LM_OK;
}

static bool should_be_resized(table_t *table)
{
	return table->size >= table->max_size / 2;
}

table_status_t table_init(table_t *table, size_t capacity, size_t increment,
						  hash_fn_t hash)
{
	if (!table) { return LM_CANNOT_INIT_TABLE; }
	
	table->size = 0;
	table->max_size = capacity;
	table->increment = increment;
	table->pairs = malloc(capacity * sizeof(pair_t));
	table->hash = hash;
	if (table->pairs == NULL) { return LM_CANNOT_INIT_TABLE_PAIRS; }

	for (size_t i = 0; i < capacity; i++) {
		table->pairs[i].key = 0;
		table->pairs[i].value = NULL;
	}

	return LM_OK;
}

table_status_t table_put(table_t *table, uint32_t key,
						 const cacheidx_arr_t *val)
{
	uint32_t hash;
	size_t idx;

	if (should_be_resized(table)) {
		table_status_t res = increase_table_size(table);
		if (res != LM_OK) { return res; }
	}

	hash = table->hash(&key);

	for (size_t i = 0, max_size = table->max_size; i < max_size; i++) {
		idx = (hash + i) % (max_size - 1);
		if (!table->pairs[idx].value) {
			cacheidx_arr_t *new_arr = cache_array_copy(val);
			table->pairs[idx] = (pair_t) {.key = key, .value = new_arr};
			table->size++;
			return LM_OK;
		}
	}
	return LM_CANNOT_INSERT_VALUE;
}

table_status_t table_add(table_t *table, uint32_t key, cache_index_t *val)
{
	uint32_t hash;
	size_t idx;

	hash = table->hash(&key);

	for (size_t i = 0, max_size = table->max_size; i < max_size; i++) {
		idx = (hash + i) % (max_size - 1);
		if (table->pairs[idx].key == key) {
			if (!table->pairs[idx].value) { return LM_CANNOT_INSERT_VALUE; }
			if (cache_array_add(table->pairs[idx].value, val) != OK) {
				(void)log_err("table_add: an error occurred while adding the "
							  "cache index for commit `%s` to the map\n",
							  val->hash);
				return LM_CANNOT_INSERT_VALUE;
			}
			return LM_OK;
		}
	}
	return LM_CANNOT_INSERT_VALUE;
}

cacheidx_arr_t *table_get(const table_t * table, uint32_t key)
{
	table_status_t hash;
	size_t idx;

	hash = table->hash(&key);
	for (size_t i = 0, max_size = table->max_size; i < max_size; i++) {
		idx = (hash + i) % (max_size - 1);
		if (table->pairs[idx].key == key) {
			return table->pairs[idx].value;
		}
	}

	return NULL;
}

table_status_t table_remove(table_t * table, uint32_t key)
{
	uint32_t hash;
	size_t idx;

	hash = table->hash(&key);

	for (size_t i = 0, max_size = table->max_size; i < max_size; i++) {
		idx = (hash + i) % (max_size - 1);
		if (table->pairs[idx].key == key) {
			str_array_free(&table->pairs[idx].value);
			table->pairs[idx].key = 0;
			table->size--;
			return LM_OK;
		}
	}
	return LM_CANNOT_REMOVE_NONEXISTENT_VALUE;
}

table_status_t table_get_pairs(const table_t *table, pair_t *keys)
{
	size_t counter = 0;

	if (!keys) { return LM_NULL_KEY_ARRAY; }

	for (size_t i = 0, max_size = table->max_size; i < max_size; i++) {
		if (table->pairs[i].value) {
			keys[counter].key = table->pairs[i].key;
			keys[counter].value = table->pairs[i].value;
			counter++;
		}
	}

	return LM_OK;
}

static void free_pair(pair_t *pair)
{
	if (pair->value) {
		str_array_free(&pair->value);
	}
}

void table_free(table_t *table)
{
	for (size_t i = 0; i < table->max_size; i++) {
		free_pair(table->pairs + i);
	}
	free(table->pairs);
	table->pairs = NULL;
	table->increment = 0;
	table->max_size = 0;
	table->size = 0;
}

bool chache_idx_equal(const cache_index_t *id1, const cache_index_t *id2)
{
	return str_equals(id1->hash, id2->hash)
		   && id1->is_favorite == id2->is_favorite;
}

void cache_idx_free(cache_index_t *idx)
{
	str_free(idx->hash);
}

/*
 * Cache index arrays
 */
static void assign_cache_idx(void *src, void *elem)
{
	cache_index_t *index = (cache_index_t *)elem;
	*(cache_index_t *)src = (cache_index_t) {
		.hash = str_copy(index->hash),
		.is_favorite = index->is_favorite,
	};
}

static int compare_cache_idx(void *idx1, void *idx2)
{
	cache_index_t *index1 = (cache_index_t *)idx1;
	cache_index_t *index2 = (cache_index_t *)idx2;
	return str_compare(index1->hash, index2->hash);
}

static void free_cache_idx(void* idx)
{
	cache_index_t *index = (cache_index_t *)idx;
	str_free(index->hash);
}

void cache_array_init(cacheidx_arr_t **arr)
{
	array_init(arr, sizeof(cache_index_t));
}

cache_index_t *cache_array_get(const cacheidx_arr_t *src, size_t i)
{
	return (cache_index_t *)src->values + i;
}

return_code_t cache_array_add(cacheidx_arr_t *src, cache_index_t *idx)
{
	return array_add(src, idx, assign_cache_idx);
}

cacheidx_arr_t *cache_array_copy(const cacheidx_arr_t *src)
{
	return array_copy(src, assign_cache_idx);
}

bool cache_array_contains(const cacheidx_arr_t *src, cache_index_t *idx)
{
	return array_contains(src, idx, compare_cache_idx);
}

void cache_array_free(cacheidx_arr_t **arr)
{
	array_free(arr, free_cache_idx);
}
