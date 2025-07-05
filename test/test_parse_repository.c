/* test_parse_repository.c
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
#include "../src/repo.h"
#include "../src/str.h"

#include <stdlib.h>
#include <string.h>

void test_parse_repository()
{
	{
		const char *line = "repo/path[https://example.com/]";
		repository_t repo = parse_repository(line, strlen(line), 0);
		
		assert_true(str_arr_equals(repo.path, "repo/path"), "Path should be 'repo/path'");
		assert_true(str_arr_equals(repo.url, "https://example.com/"), "URL should be 'https://example.com/'");

		str_free(repo.path);
		str_free(repo.name);
		str_free(repo.url);
	}
	
	{
		const char *line = "path/to/repo";
		repository_t repo = parse_repository(line, strlen(line), 0);
		
		assert_true(str_arr_equals(repo.path, "path/to/repo"), "Path should be 'path/to/repo'");
		assert_true(repo.url.len == 0, "URL should be empty");

		str_free(repo.path);
		str_free(repo.name);
		str_free(repo.url);
	}
	
	{
		const char *line = "repo/path[https://example.com";
		repository_t repo = parse_repository(line, strlen(line), 0);
		
		assert_true(str_arr_equals(repo.path, "repo/path"), "Path should be 'repo/path'");
		assert_true(str_arr_equals(repo.url, "https://example.com/"), "URL should be 'https://example.com/'");

		str_free(repo.path);
		str_free(repo.name);
		str_free(repo.url);
	}
	
	{
		const char *line = "[https://example.com]";
		repository_t repo = parse_repository(line, strlen(line), 0);
		
		assert_true(repo.path.len == 0, "Path should be empty");
		assert_true(str_arr_equals(repo.url, "https://example.com/"), "URL should be 'https://example.com/'");

		str_free(repo.path);
		str_free(repo.name);
		str_free(repo.url);
	}
	
	{
		const char *line = "repo/path[]";
		repository_t repo = parse_repository(line, strlen(line), 0);
		
		assert_true(str_arr_equals(repo.path, "repo/path"), "Path should be 'repo/path'");
		assert_true(repo.url.len == 0, "URL should be empty");

		str_free(repo.path);
		str_free(repo.name);
		str_free(repo.url);
	}
	
	{
		const char *line = "repo/path[https://example.com/repo[name]]";
		repository_t repo = parse_repository(line, strlen(line), 0);
		
		assert_true(str_arr_equals(repo.path, "repo/path"), "Path should be 'repo/path'");
		assert_true(str_arr_equals(repo.url, "https://example.com/repo[name]/"), "URL should be 'https://example.com/repo[name]/'");

		str_free(repo.path);
		str_free(repo.name);
		str_free(repo.url);
	}
	
	{
		const char *line = "";
		repository_t repo = parse_repository(line, strlen(line), 0);
		
		assert_true(repo.path.len == 0, "Path should be empty");
		assert_true(repo.url.len == 0, "URL should be empty");

		str_free(repo.path);
		str_free(repo.name);
		str_free(repo.url);
	}
	
	{
		const char *line = "repo/path with spaces[https://example.com/]";
		repository_t repo = parse_repository(line, strlen(line), 0);
		
		assert_true(str_arr_equals(repo.path, "repo/path with spaces"), "Path should be 'repo/path with spaces'");
		assert_true(str_arr_equals(repo.url, "https://example.com/"), "URL should be 'https://example.com/'");

		str_free(repo.path);
		str_free(repo.name);
		str_free(repo.url);
	}
	
	{
		const char *line = "repo/path[git@github.com:user/repo.git]";
		repository_t repo = parse_repository(line, strlen(line), 0);
		
		assert_true(str_arr_equals(repo.path, "repo/path"), "Path should be 'repo/path'");
		assert_true(str_arr_equals(repo.url, "git@github.com:user/repo.git/"), "URL should be 'git@github.com:user/repo.git/'");

		str_free(repo.path);
		str_free(repo.name);
		str_free(repo.url);
	}

	/* Added tests for branches */
	{
		const char *line = "repo/path:main[https://example.com/]";
		repository_t repo = parse_repository(line, strlen(line), 0);
		
		assert_true(str_arr_equals(repo.path, "repo/path"), "Path should be 'repo/path'");
		assert_true(str_arr_equals(repo.url, "https://example.com/"), "URL should be 'https://example.com/'");
		assert_true(repo.branches->len == 1, "Should have one branch");
		assert_true(str_arr_equals(repo.branches->strings[0], "main"), "Branch should be 'main'");

		str_free(repo.path);
		str_free(repo.name);
		str_free(repo.url);
		str_array_free(&repo.branches);
	}

	{
		const char *line = "repo/path:main,dev,feature-x[https://example.com/]";
		repository_t repo = parse_repository(line, strlen(line), 0);

		assert_true(str_arr_equals(repo.path, "repo/path"), "Path should be 'repo/path'");
		assert_true(str_arr_equals(repo.url, "https://example.com/"), "URL should be 'https://example.com/'");
		assert_true(repo.branches->len == 3, "Should have three branches");
		assert_true(str_arr_equals(repo.branches->strings[0], "main"), "Branch[0] should be 'main'");
		assert_true(str_arr_equals(repo.branches->strings[1], "dev"), "Branch[1] should be 'dev'");
		assert_true(str_arr_equals(repo.branches->strings[2], "feature-x"), "Branch[2] should be 'feature-x'");

		str_free(repo.path);
		str_free(repo.name);
		str_free(repo.url);
		str_array_free(&repo.branches);
	}

	{
		const char *line = ":main[https://example.com/]";
		repository_t repo = parse_repository(line, strlen(line), 0);

		assert_true(str_arr_equals(repo.path, ""), "Path should be empty");
		assert_true(str_arr_equals(repo.url, "https://example.com/"), "URL should be 'https://example.com/'");
		assert_true(repo.branches->len == 1, "Should have one branch");
		assert_true(str_arr_equals(repo.branches->strings[0], "main"), "Branch should be 'main'");

		str_free(repo.path);
		str_free(repo.name);
		str_free(repo.url);
		str_array_free(&repo.branches);
	}

	{
		const char *line = "repo/path:main[]";
		repository_t repo = parse_repository(line, strlen(line), 0);

		assert_true(str_arr_equals(repo.path, "repo/path"), "Path should be 'repo/path'");
		assert_true(str_arr_equals(repo.url, ""), "URL should be empty");
		assert_true(repo.branches->len == 1, "Should have one branch");
		assert_true(str_arr_equals(repo.branches->strings[0], "main"), "Branch should be 'main'");

		str_free(repo.path);
		str_free(repo.name);
		str_free(repo.url);
		str_array_free(&repo.branches);
	}
}

int main(void)
{
	test_parse_repository();
	print_report();
}
