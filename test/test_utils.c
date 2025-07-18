/* test_utils.c
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
#include "../src/codes.h"
#include "../src/str.h"

#include <stdlib.h>
#include <string.h>
 
void test_header_of_text(void)
{
	{
		str_t text = str_init("First row\nSecond row\nThird row",
							  strlen("First row\nSecond row\nThird row"));
		str_t result = get_first_line(text);

		assert_true(result.len == 9, "first line of text should have length 10");
		assert_true(str_arr_equals(result, "First row"), "first line is 'First row'");

		str_free(text);
		str_free(result);
	}

	{
		str_t text = str_init("A row without a newline",
							  strlen("A row without a newline"));
		str_t result = get_first_line(text);

		assert_true(result.len == text.len,
					"the first line has the same length of to the text itself");
		assert_true(str_equals(result, text),
					"the first line is equal to the text itself");
		
		str_free(text);
		str_free(result);
	}

	{
		str_t text = empty_str();
		str_t result = get_first_line(text);

		assert_true(result.len == 0, "empty text has length 0");
		assert_true(str_equals(result, text),
					"empty text is equal to the empty string");

		str_free(text);
		str_free(result);
	}

	{
		str_t text = str_init("\nSecond row", strlen("\nSecond row"));
		str_t result = get_first_line(text);
		str_t empty = empty_str();

		assert_true(result.len == 0, "the first line should have length 0");
		assert_true(str_equals(result, empty),
					"the first line is the empty string");

		str_free(text);
		str_free(result);
		str_free(empty);
	}
}

void test_format_date(void) 
{
	{
		time_t now = time(NULL);
		str_t result = format_date(now, true);
		
		assert_true(result.len == 12, DATE_PATTERN "has 10 characters");
		assert_true(result.val[3] == ASCII_SPACE, "there should be 3 chars for month");
		assert_true(result.val[6] == ',', "there should be 7 chars for month and day");

		str_free(result);
	}

	{
		time_t timestamp = 1732838400;
		str_t result = format_date(timestamp, true);
		
		assert_true(result.len == 12, "'Nov 29, 2024' has 12 characters");
		assert_true(str_arr_equals(result, "Nov 29, 2024"), "date should be 'Nov 29, 2024'");

		str_free(result);
	}

	{
		time_t timestamp = 1578528000;
		str_t result = format_date(timestamp, true);
		
		assert_true(result.len == 12, "'Jan 09, 2020' has 12 characters");
		assert_true(str_arr_equals(result, "Jan 09, 2020"), "date should be 'Jan 09, 2020'");

		str_free(result);
	}

	{
		time_t timestamp = 4101840000;
		str_t result = format_date(timestamp, true);
		
		assert_true(result.len == 12, "'Dec 25, 2099' has 12 characters");
		assert_true(str_arr_equals(result, "Dec 25, 2099"), "date should be 'Dec 25, 2099'");

		str_free(result);
	}

	{
		time_t timestamp = 1709164800;
		str_t result = format_date(timestamp, true);

		assert_true(result.len == 12, "'Feb 29, 2024' has 12 characters");
		assert_true(str_arr_equals(result, "Feb 29, 2024"),
					"Leap year (2024), date should be 'Feb 29, 2024'");

		str_free(result);
	}
}

void test_escape_special_chars(void) 
{
	{
		str_t input = str_init("hello_world", 11);
		str_t result = escape_special_chars(input);
		assert_true(str_arr_equals(result, "hello\\_world"),
					"single underscore escaped correctly");
		
		str_free(input);
		str_free(result);
	}
	
	{
		str_t input = str_init("__test__", 8);
		str_t result = escape_special_chars(input);
		assert_true(str_arr_equals(result, "\\_\\_test\\_\\_"),
					"double underscore escaped correctly");
		
		str_free(input);
		str_free(result);
	}
	
	{
		str_t input = str_init("normal", 6);
		str_t result = escape_special_chars(input);
		assert_true(str_arr_equals(result, "normal"), "nothing to escape");
		
		str_free(input);
		str_free(result);
	}
	
	{
		str_t input = str_init("#delete#", 8);
		str_t result = escape_special_chars(input);
		assert_true(str_arr_equals(result, "\\#delete\\#"),
					"single hash escaped");
		
		str_free(input);
		str_free(result);
	}
   
	{
		str_t input = str_init("## Markdown subtitle", 20);
		str_t result = escape_special_chars(input);
		assert_true(str_arr_equals(result, "\\#\\# Markdown subtitle"),
					"double hash escaped");
		
		str_free(input);
		str_free(result);
	}
	
	{
		str_t input = str_init("mixed_#chars", 12);
		str_t result = escape_special_chars(input);
		assert_true(str_arr_equals(result, "mixed\\_\\#chars"),
					"combo of hash and underscore escaped");
		
		str_free(input);
		str_free(result);
	}
	
	{
		str_t input = str_init("", 0);
		str_t result = escape_special_chars(input);
		assert_true(str_arr_equals(result, ""),
					"empty string, nothing to escape");
		
		str_free(input);
		str_free(result);
	}
}

void test_trim_whitespace(void)
{
	{
		char *input = "\t\n  test string  \n\t";
		char *expected = "test string";
		char *result = trim_whitespace(input);

		assert_true(strcmp(result, expected) == 0,
					"new line and tabs should be trimmed");
		free(result);
	}

	{
		char *input = "nospaces";
		char *expected = "nospaces";
		char *result = trim_whitespace(input);

		assert_true(strcmp(result, expected) == 0,
					"'nospaces' should not be trimmed");
		free(result);
	}

	{
		char *input = "    ";
		char *expected = "";
		char *result = trim_whitespace(input);

		assert_true(strcmp(result, expected) == 0,
					"a space string should become the empty sring");
		free(result);
	}

	{
		char *input = "";
		char *expected = "";
		char *result = trim_whitespace(input);

		assert_true(strcmp(result, expected) == 0,
					"empty string should remain the empty string");
		free(result);
	}

	{
		char *input = "   leading";
		char *expected = "leading";
		char *result = trim_whitespace(input);

		assert_true(strcmp(result, expected) == 0,
					"should trim the leading whitespaces");
		free(result);
	}

	{
		char *input = "trailing   ";
		char *expected = "trailing";
		char *result = trim_whitespace(input);

		assert_true(strcmp(result, expected) == 0,
					"should trim the trailing whitespaces");
		free(result);
	}

	{
		char *input = "   hello   ";
		char *expected = "hello";
		char *result = trim_whitespace(input);

		assert_true(strcmp(result, expected) == 0,
					"should trim both the leading and the trailing whitespaces");
		free(result);
	}
}

void test_parse_commit_id(void)
{
	{
		const char *line = "+ 12345) RepoName";
		unsigned id = 0;
		return_code_t result = parse_commit_id(&id, line);

		assert_true(result == OK && id == 12345,
					"should correctly parse valid input with ID 12345");
	}

	{
		const char *line = "+ 0) ZeroID";
		unsigned id = 999;
		return_code_t result = parse_commit_id(&id, line);

		assert_true(result == OK && id == 0,
					"should correctly parse zero as ID");
	}

	{
		const char *line = "+12345)NoSpace";
		unsigned id = 0;
		return_code_t result = parse_commit_id(&id, line);

		assert_true(result == COMMITS_FILE_INVALID_REPO_ID,
					"should reject input without space after '+'");
	}

	{
		const char *line = "12345) MissingPlus";
		unsigned id = 0;
		return_code_t result = parse_commit_id(&id, line);

		assert_true(result == COMMITS_FILE_INVALID_REPO_ID,
					"should reject input without '+' at the start");
	}

	{
		const char *line = "+ abc) NotANumber";
		unsigned id = 0;
		return_code_t result = parse_commit_id(&id, line);

		assert_true(result == COMMITS_FILE_INVALID_REPO_ID,
					"should reject input with non-digit ID");
	}

	{
		const char *line = "+ 12345 NoClosingParen";
		unsigned id = 0;
		return_code_t result = parse_commit_id(&id, line);

		assert_true(result == COMMITS_FILE_INVALID_REPO_ID,
					"should reject input without closing parenthesis");
	}

	{
		const char *line = "+ 123)extra)stuff";
		unsigned id = 0;
		return_code_t result = parse_commit_id(&id, line);

		assert_true(result == OK && id == 123,
					"should accept valid ID even with extra characters after ')'");
	}

	{
		const char *line = NULL;
		unsigned id = 0;
		return_code_t result = parse_commit_id(&id, line);

		assert_true(result == COMMITS_FILE_INVALID_REPO_ID,
					"should reject NULL line input");
	}

	{
		const char *line = "+ 123)";
		return_code_t result = parse_commit_id(NULL, line);

		assert_true(result == COMMITS_FILE_INVALID_REPO_ID,
					"should reject NULL id pointer");
	}
}

int main(void)
{
	test_header_of_text();
	test_format_date();
	test_escape_special_chars();
	test_trim_whitespace();
	test_parse_commit_id();
	print_report();
}
