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
#include "../src/lookup_table.h"
#include "../src/repo.h"
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

repository_t make_repo(unsigned id, const char *name, const char *url, const char *path)
{
	repository_t repo;
	repo.id = id;
	repo.name = str_init(name, strlen(name));
	repo.url = str_init(url, strlen(url));
	repo.path = str_init(path, strlen(path));
	repo.branches = NULL;
	repo.format = (fmt_t){0};
	repo.history = NULL;
	return repo;
}

void test_repo_array_add_get(void)
{
	repository_array_t *arr = NULL;
	repo_array_init(&arr);

	repository_t r1 = make_repo(1, "repo1", "https://github.com/repo1", "/tmp/repo1");
	assert_true(repo_array_add(arr, &r1) == OK, "repo_array_add should return OK");

	repository_t *r = repo_array_get(arr, 0);
	assert_true(str_arr_equals(r->name, "repo1"), "name of first repo should be 'repo1'");
	assert_true(r->id == 1, "id should be 1");

	str_free(r1.name);
	str_free(r1.url);
	str_free(r1.path);
	repo_array_free(&arr);
}

void test_repo_array_contains(void)
{
	repository_array_t *arr = NULL;
	repo_array_init(&arr);

	repository_t r1 = make_repo(1, "repo1", "https://github.com/repo1", "/tmp/repo1");
	repo_array_add(arr, &r1);

	repository_t r2 = make_repo(1, "repo1", "otherurl", "otherpath");
	assert_true(repo_array_contains(arr, &r2), "repo_array_contains should match on id");

	str_free(r1.name);
	str_free(r1.url);
	str_free(r1.path);
	str_free(r2.name);
	str_free(r2.url);
	str_free(r2.path);
	repo_array_free(&arr);
}

void test_repo_array_copy(void)
{
	repository_array_t *arr = NULL;
	repo_array_init(&arr);

	repository_t r1 = make_repo(1, "repo1", "https://github.com/repo1", "/tmp/repo1");
	repo_array_add(arr, &r1);

	repository_array_t *copy = repo_array_copy(arr);

	assert_true(copy != NULL, "copy should not be NULL");
	assert_true(copy->len == arr->len, "copy should have same length as original");
	assert_true(str_equals(repo_array_get(copy, 0)->name, r1.name), "first repo name in copy should match original");

	str_free(r1.name);
	str_free(r1.url);
	str_free(r1.path);
	repo_array_free(&arr);
	repo_array_free(&copy);
}

void test_repo_array_empty_contains(void)
{
	repository_array_t *arr = NULL;
	repo_array_init(&arr);

	repository_t r = make_repo(2, "repo2", "url", "path");
	assert_true(!repo_array_contains(arr, &r), "empty array should not contain any repo");

	str_free(r.name);
	str_free(r.url);
	str_free(r.path);
	repo_array_free(&arr);
}

void test_repo_array_multiple_add(void)
{
	repository_array_t *arr = NULL;
	repo_array_init(&arr);

	for (int i = 0; i < 10; i++) {
		char name[16];
		snprintf(name, sizeof(name), "repo%d", i);
		repository_t r = make_repo(i, name, "url", "path");
		assert_true(repo_array_add(arr, &r) == OK, "repo_array_add should return OK for multiple repos");
		str_free(r.name);
		str_free(r.url);
		str_free(r.path);
	}

	assert_true(arr->len == 10, "array len should be 10");
	repo_array_free(&arr);
}

void test_cache_array_add_get(void)
{
	cacheidx_arr_t *arr = NULL;
	cache_array_init(&arr);

	cache_index_t idx = {
		.hash = str_init("hash1", 5),
		.is_favorite = true
	};
	assert_true(cache_array_add(arr, &idx) == OK, "cache_array_add should return OK");

	cache_index_t *got = cache_array_get(arr, 0);
	assert_true(str_arr_equals(got->hash, "hash1"), "hash of first index should be 'hash1'");
	assert_true(got->is_favorite == true, "is_favorite should be true");

	str_free(idx.hash);
	cache_array_free(&arr);
}

void test_cache_array_contains(void)
{
	cacheidx_arr_t *arr = NULL;
	cache_array_init(&arr);

	cache_index_t idx = {
		.hash = str_init("hash1", 5),
		.is_favorite = false
	};
	cache_array_add(arr, &idx);

	cache_index_t idx2 = {
		.hash = str_init("hash1", 5),
		.is_favorite = false
	};
	assert_true(cache_array_contains(arr, &idx2), "cache_array_contains should match on hash");

	str_free(idx.hash);
	str_free(idx2.hash);
	cache_array_free(&arr);
}

void test_cache_array_copy(void)
{
	cacheidx_arr_t *arr = NULL;
	cache_array_init(&arr);

	cache_index_t idx = {
		.hash = str_init("hash1", 5),
		.is_favorite = false
	};
	cache_array_add(arr, &idx);

	cacheidx_arr_t *copy = cache_array_copy(arr);

	assert_true(copy != NULL, "copy should not be NULL");
	assert_true(copy->len == arr->len, "copy should have same length as original");
	assert_true(str_equals(cache_array_get(copy, 0)->hash, idx.hash), "first hash in copy should match original");

	str_free(idx.hash);
	cache_array_free(&arr);
	cache_array_free(&copy);
}

void test_cache_array_empty_contains(void)
{
	cacheidx_arr_t *arr = NULL;
	cache_array_init(&arr);

	cache_index_t idx = {
		.hash = str_init("hash2", 5),
		.is_favorite = false
	};
	assert_true(!cache_array_contains(arr, &idx), "empty array should not contain any index");

	str_free(idx.hash);
	cache_array_free(&arr);
}

void test_cache_array_multiple_add(void)
{
	cacheidx_arr_t *arr = NULL;
	cache_array_init(&arr);

	for (int i = 0; i < 10; i++) {
		char hash[16];
		snprintf(hash, sizeof(hash), "hash%d", i);
		cache_index_t idx = {
			.hash = str_init(hash, strlen(hash)),
			.is_favorite = (i % 2 == 0)
		};
		assert_true(cache_array_add(arr, &idx) == OK, "cache_array_add should return OK for multiple indexes");
		str_free(idx.hash);
	}

	assert_true(arr->len == 10, "array len should be 10");
	cache_array_free(&arr);
}

int main(void)
{
	/* commit array */
	test_commit_array_add_get();
	test_commit_array_contains();
	test_commit_array_copy();
	test_commit_array_empty_contains();
	test_commit_array_multiple_add();

	/* repository array */
	test_repo_array_add_get();
	test_repo_array_contains();
	test_repo_array_copy();
	test_repo_array_empty_contains();
	test_repo_array_multiple_add();

	/* cache index array */
	test_cache_array_add_get();
	test_cache_array_contains();
	test_cache_array_copy();
	test_cache_array_empty_contains();
	test_cache_array_multiple_add();

	print_report();
}
