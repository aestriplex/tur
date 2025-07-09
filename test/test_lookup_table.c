/* test_lookup_table.c
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

#include "test.h"
#include "../src/lookup_table.h"
#include "../src/str.h"

#include <string.h>

/* Simple hash for testing: just return the key as uint32_t */
uint32_t simple_hash(void *key)
{
	return *(uint32_t *)key;
}

void test_table_init(void)
{
	table_t table;
	table_status_t status = table_init(&table, 10, 10, simple_hash);
	assert_true(status == LM_OK, "table_init should return LM_OK");

	bool arrays_init_to_null = 1;
	bool keys_init_to_zero = 1;
	for (size_t i = 0; i < table.max_size; i++) {
		arrays_init_to_null &= table.pairs[i].value == NULL;
		keys_init_to_zero &= table.pairs[i].key == 0;
	}
	assert_true(arrays_init_to_null, "table_init should initialize all the "
									 "values (string arrays) to NULL");
	assert_true(keys_init_to_zero, "table_init should initialize all the "
								   "keys to 0");

	table_free(&table);
}

void test_table_put_and_get(void)
{
	table_t table;
	cache_index_t idx = (cache_index_t) {
		.hash = str_init("08d9375380f58077f1335af168c7d486e686bc37", 40),
		.is_favorite = false,
	};

	table_status_t status = table_init(&table, 10, 10, simple_hash);

	cacheidx_arr_t *arr = NULL;
	cache_array_init(&arr);
	cache_array_add(arr, &idx);

	status = table_put(&table, 42, arr);
	assert_true(status == LM_OK, "table_put should return LM_OK");

	cacheidx_arr_t *got = table_get(&table, 42);
	assert_true(got != NULL, "table_get should return non-NULL for existing key");
	assert_true(got->len == 1, "cacheidx_arr_t should have length 1");
	assert_true(chache_idx_equal(cache_array_get(got, 0), &idx),
				"returned commit array should be equal to the one inserted");

	cache_idx_free(&idx);
	cache_array_free(&arr);
	table_free(&table);
}

void test_table_add_and_get(void)
{
	table_t table;
	cache_index_t idx1 = (cache_index_t) {
		.hash = str_init("4df35a923c71fd92afb99bb6166138ffc61b6ce6", 40),
		.is_favorite = true,
	};
	cache_index_t idx2 = (cache_index_t) {
		.hash = str_init("fd21c7fcaa94a9ee9767eed0e4705983d64fd609", 40),
		.is_favorite = true,
	};

	table_init(&table, 10, 10, simple_hash);

	cacheidx_arr_t *arr = NULL;
	cache_array_init(&arr);
	cache_array_add(arr, &idx1);

	table_put(&table, 7, arr);

	table_add(&table, 7, &idx2);
	
	cacheidx_arr_t *got = table_get(&table, 7);
	assert_true(got != NULL, "table_get should return non-NULL after add");
	assert_true(got->len == 2, "cache array should have length 2");
	assert_true(chache_idx_equal(cache_array_get(got, 1), &idx2),
				"second commit should be equal to idx2");
	
	cache_idx_free(&idx1);
	cache_idx_free(&idx2);
	cache_array_free(&arr);
	table_free(&table);
}

void test_table_remove(void)
{
	table_t table;
	cache_index_t idx = (cache_index_t) {
		.hash = str_init("4df35a923c71fd92afb99bb6166138ffc61b6ce6", 40),
		.is_favorite = false,
	};

	table_init(&table, 10, 10, simple_hash);

	cacheidx_arr_t *arr = NULL;
	cache_array_init(&arr);
	cache_array_add(arr, &idx);

	table_put(&table, 99, arr);

	table_status_t status = table_remove(&table, 99);
	assert_true(status == LM_OK, "table_remove should return LM_OK");

	cacheidx_arr_t *got = table_get(&table, 99);
	assert_true(got == NULL, "table_get should return NULL after remove");

	cache_idx_free(&idx);
	str_array_free(&arr);
	table_free(&table);
}

void test_table_get_pairs(void)
{
	table_t table;
	cache_index_t idx1 = (cache_index_t) {
		.hash = str_init("4df35a923c71fd92afb99bb6166138ffc61b6ce6", 40),
		.is_favorite = false,
	};
	cache_index_t idx2 = (cache_index_t) {
		.hash = str_init("fd21c7fcaa94a9ee9767eed0e4705983d64fd609", 40),
		.is_favorite = true,
	};

	table_init(&table, 10, 10, simple_hash);

	cacheidx_arr_t *arr1 = NULL;
	cache_array_init(&arr1);
	cache_array_add(arr1, &idx1);

	table_put(&table, 1, arr1);

	cacheidx_arr_t *arr2 = NULL;
	cache_array_init(&arr2);
	cache_array_add(arr2, &idx2);

	table_put(&table, 2, arr2);

	pair_t pairs[2];
	table_status_t status = table_get_pairs(&table, pairs);
	assert_true(status == LM_OK, "table_get_pairs should return LM_OK");
	assert_true(pairs[0].key == 1 || pairs[1].key == 1, "key 1 should be present");
	assert_true(pairs[0].key == 2 || pairs[1].key == 2, "key 2 should be present");

	cache_idx_free(&idx1);
	cache_idx_free(&idx2);	
	str_array_free(&arr1);
	str_array_free(&arr2);
	table_free(&table);
}

int main(void) {
	test_table_init();
	test_table_put_and_get();
	test_table_add_and_get();
	test_table_remove();
	test_table_get_pairs();
	print_report();
}
