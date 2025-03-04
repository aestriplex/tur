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

#include <string.h>

void test_parse_repository()
{
	{
		const char *line = "repo/path[https://example.com/]";
		repository_t repo = parse_repository(line, strlen(line));
		
		assert_true(str_arr_equals(repo.path, "repo/path"), "Path should be 'repo/path'");
		assert_true(str_arr_equals(repo.url, "https://example.com/"), "URL should be 'https://example.com/'");
	}
	
	{
		const char *line = "path/to/repo";
		repository_t repo = parse_repository(line, strlen(line));
		
		assert_true(str_arr_equals(repo.path, "path/to/repo"), "Path should be 'path/to/repo'");
		assert_true(repo.url.len == 0, "URL should be empty");
	}
	
	{
		const char *line = "repo/path[https://example.com";
		repository_t repo = parse_repository(line, strlen(line));
		
		assert_true(str_arr_equals(repo.path, "repo/path"), "Path should be 'repo/path'");
		assert_true(str_arr_equals(repo.url, "https://example.com/"), "URL should be 'https://example.com/'");
	}
	
	{
		const char *line = "[https://example.com]";
		repository_t repo = parse_repository(line, strlen(line));
		
		assert_true(repo.path.len == 0, "Path should be empty");
		assert_true(str_arr_equals(repo.url, "https://example.com/"), "URL should be 'https://example.com/'");
	}
	
	{
		const char *line = "repo/path[]";
		repository_t repo = parse_repository(line, strlen(line));
		
		assert_true(str_arr_equals(repo.path, "repo/path"), "Path should be 'repo/path'");
		assert_true(repo.url.len == 0, "URL should be empty");
	}
	
	{
		const char *line = "repo/path[https://example.com/repo[name]]";
		repository_t repo = parse_repository(line, strlen(line));
		
		assert_true(str_arr_equals(repo.path, "repo/path"), "Path should be 'repo/path'");
		assert_true(str_arr_equals(repo.url, "https://example.com/repo[name]/"), "URL should be 'https://example.com/repo[name]/'");
	}
	
	{
		const char *line = "";
		repository_t repo = parse_repository(line, strlen(line));
		
		assert_true(repo.path.len == 0, "Path should be empty");
		assert_true(repo.url.len == 0, "URL should be empty");
	}
	
	{
		const char *line = "repo/path with spaces[https://example.com/]";
		repository_t repo = parse_repository(line, strlen(line));
		
		assert_true(str_arr_equals(repo.path, "repo/path with spaces"), "Path should be 'repo/path with spaces'");
		assert_true(str_arr_equals(repo.url, "https://example.com/"), "URL should be 'https://example.com/'");
	}
	
	{
		const char *line = "repo/path[git@github.com:user/repo.git]";
		repository_t repo = parse_repository(line, strlen(line));
		
		assert_true(str_arr_equals(repo.path, "repo/path"), "Path should be 'repo/path'");
		assert_true(str_arr_equals(repo.url, "git@github.com:user/repo.git/"), "URL should be 'git@github.com:user/repo.git/'");
	}
}

int main(void)
{
	test_parse_repository();
}
