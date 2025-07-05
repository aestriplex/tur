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
	table_status_t status = table_init(&table, 10, 10, simple_hash);

	str_array_t *arr = malloc(sizeof(str_array_t));
	arr->strings = malloc(2 * sizeof(str_t));
	arr->len = 1;
	arr->capacity = 2;
	arr->strings[0] = str_init("hello", 5);

	status = table_put(&table, 42, arr);
	assert_true(status == LM_OK, "table_put should return LM_OK");

	str_array_t *got = table_get(&table, 42);
	assert_true(got != NULL, "table_get should return non-NULL for existing key");
	assert_true(got->len == 1, "str_array_t should have length 1");
	assert_true(str_arr_equals(got->strings[0], "hello"), "string should be 'hello'");

	str_array_free(&arr);
	table_free(&table);
}

void test_table_add_and_get(void)
{
	table_t table;
	table_init(&table, 10, 10, simple_hash);

	str_array_t *arr = malloc(sizeof(str_array_t));
	arr->strings = malloc(2 * sizeof(str_t));
	arr->len = 1;
	arr->capacity = 2;
	arr->strings[0] = str_init("first", 5);
	table_put(&table, 7, arr);

	str_t second = str_init("second", 6);
	table_add(&table, 7, second);
	str_free(second);

	str_array_t *got = table_get(&table, 7);
	assert_true(got != NULL, "table_get should return non-NULL after add");
	assert_true(got->len == 2, "str_array_t should have length 2");
	assert_true(str_arr_equals(got->strings[1], "second"), "second string should be 'second'");

	str_array_free(&arr);
	table_free(&table);
}

void test_table_remove(void)
{
	table_t table;
	table_init(&table, 10, 10, simple_hash);

	str_array_t *arr = malloc(sizeof(str_array_t));
	arr->strings = malloc(1 * sizeof(str_t));
	arr->len = 1;
	arr->capacity = 1;
	arr->strings[0] = str_init("bye", 3);
	table_put(&table, 99, arr);

	table_status_t status = table_remove(&table, 99);
	assert_true(status == LM_OK, "table_remove should return LM_OK");

	str_array_t *got = table_get(&table, 99);
	assert_true(got == NULL, "table_get should return NULL after remove");

	str_array_free(&arr);
	table_free(&table);
}

void test_table_get_pairs(void)
{
	table_t table;
	table_init(&table, 10, 10, simple_hash);

	str_array_t *arr1 = malloc(sizeof(str_array_t));
	arr1->strings = malloc(1 * sizeof(str_t));
	arr1->len = 1;
	arr1->capacity = 1;
	arr1->strings[0] = str_init("foo", 3);
	table_put(&table, 1, arr1);

	str_array_t *arr2 = malloc(sizeof(str_array_t));
	arr2->strings = malloc(1 * sizeof(str_t));
	arr2->len = 1;
	arr2->capacity = 1;
	arr2->strings[0] = str_init("bar", 3);
	table_put(&table, 2, arr2);

	pair_t pairs[2];
	table_status_t status = table_get_pairs(&table, pairs);
	assert_true(status == LM_OK, "table_get_pairs should return LM_OK");
	assert_true(pairs[0].key == 1 || pairs[1].key == 1, "key 1 should be present");
	assert_true(pairs[0].key == 2 || pairs[1].key == 2, "key 2 should be present");

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
