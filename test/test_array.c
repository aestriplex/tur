/* test_array.c
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
#include "../src/commit.h"
#include "../src/str.h"

#include <assert.h>
#include <string.h>
#include <time.h>

commit_t make_commit(const char *hash, responsability_t r, time_t date,
					 const char *msg, size_t files, size_t added, size_t removed)
{
	commit_t c;
	c.hash = str_init(hash, strlen(hash));
	c.responsability = r;
	c.date = date;
	c.msg = str_init(msg, strlen(msg));
	c.stats = (commit_stats_t){ files, added, removed };
	return c;
}

void test_commit_array_add_get(void)
{
	commit_arr_t *arr = NULL;
	commit_array_init(&arr);

	commit_t c1 = make_commit("abc123", AUTHORED, 0, "Initial commit", 1, 10, 0);
	assert_true(commit_array_add(arr, &c1) == OK, "commit_array_add should return OK");

	commit_t *c = commit_array_get(arr, 0);
	assert_true(str_arr_equals(c->hash, "abc123"), "hash of first commit should be 'abc123'");
	assert_true(c->stats.lines_added == 10, "lines_added should be 10");
	assert_true(c->responsability == AUTHORED, "responsibility should be AUTHORED");

	commit_free(&c1);
	commit_array_free(&arr);
}

void test_commit_array_contains(void)
{
	commit_arr_t *arr = NULL;
	commit_array_init(&arr);

	commit_t c1 = make_commit("abc123", AUTHORED, 0, "Initial commit", 1, 10, 0);
	commit_array_add(arr, &c1);

	commit_t c2 = make_commit("abc123", CO_AUTHORED, 100, "Something else", 0, 0, 0);
	assert_true(commit_array_contains(arr, &c2), "commit_array_contains should match on hash only");

	commit_free(&c1);
	commit_free(&c2);
	commit_array_free(&arr);
}

void test_commit_array_copy(void)
{
	commit_arr_t *arr = NULL;
	commit_array_init(&arr);

	commit_t c1 = make_commit("abc123", AUTHORED, 0, "Initial commit", 1, 10, 0);
	commit_array_add(arr, &c1);

	commit_arr_t *copy = commit_array_copy(arr);

	assert_true(copy != NULL, "copy should not be NULL");
	assert_true(copy->len == arr->len, "copy should have same length as original");
	assert_true(str_equals(commit_array_get(copy, 0)->hash, c1.hash), "first commit hash in copy should match original");

	commit_free(&c1);
	commit_array_free(&arr);
	commit_array_free(&copy);
}

void test_commit_array_empty_contains(void)
{
	commit_arr_t *arr = NULL;
	commit_array_init(&arr);

	commit_t c = make_commit("def456", AUTHORED, 0, "Missing", 0, 0, 0);
	assert_true(!commit_array_contains(arr, &c), "empty array should not contain any commit");

	commit_free(&c);
	commit_array_free(&arr);
}

void test_commit_array_multiple_add(void)
{
	commit_arr_t *arr = NULL;
	commit_array_init(&arr);

	for (int i = 0; i < 10; i++) {
		char hash[16];
		snprintf(hash, sizeof(hash), "hash%d", i);
		commit_t c = make_commit(hash, AUTHORED, i, "msg", 1, i * 2, i);
		assert_true(commit_array_add(arr, &c) == OK, "commit_array_add should return OK for multiple commits");
		commit_free(&c);
	}

	assert_true(arr->len == 10, "array len should be 10");
	commit_array_free(&arr);
}

int main(void)
{
	test_commit_array_add_get();
	test_commit_array_contains();
	test_commit_array_copy();
	test_commit_array_empty_contains();
	test_commit_array_multiple_add();
	print_report();
}
