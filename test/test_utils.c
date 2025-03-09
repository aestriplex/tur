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
		str_t result = format_date(now);

		assert_true(result.len == 10, "mm/dd/yyyy has 10 characters");
		assert_true(result.val[2] == '/', "there should be 2 chars for month");
		assert_true(result.val[5] == '/', "there should be 2 chars for day");

		str_free(result);
	}

	{
		time_t timestamp = 1732838400;
		str_t result = format_date(timestamp);
		assert_true(result.len == 10, "11/29/2024 has 10 characters");
		assert_true(str_arr_equals(result, "11/29/2024"), "date should be '11/29/2024'");

		str_free(result);
	}

	{
		time_t timestamp = 1578528000;
		str_t result = format_date(timestamp);
		assert_true(result.len == 10, "01/09/2020 has 10 characters");
		assert_true(str_arr_equals(result, "01/09/2020"), "date should be '01/09/2020'");

		str_free(result);
	}

	{
		time_t timestamp = 4101840000;
		str_t result = format_date(timestamp);
		assert_true(result.len == 10, "12/25/2099 has 10 characters");
		assert_true(str_arr_equals(result, "12/25/2099"), "date should be '12/25/2099'");

		str_free(result);
	}

	{
		time_t timestamp = 1709164800;
		str_t result = format_date(timestamp);
		assert_true(result.len == 10, "02/29/2024 has 10 characters");
		assert_true(str_arr_equals(result, "02/29/2024"),
					"Leap year (2024), date should be '02/29/2024'");

		str_free(result);
	}
}

void test_parse_optarg_to_int(void) {
	unsigned value;
	
	assert_true(parse_optarg_to_int("42", &value) == OK && value == 42,
				"parsed value is 42");
	assert_true(parse_optarg_to_int("0", &value) == OK && value == 0,
				"parsed value is 0");
	assert_true(parse_optarg_to_int("4294967295", &value) == 0 && value == 4294967295,
				"parsed value is 4294967295");
	assert_true(parse_optarg_to_int("4294967296", &value) == INT_OVERFLOW,
				"uint overflow (value 4294967296)");
	assert_true(parse_optarg_to_int("-100", &value) == USUPPORTED_NEGATIVE_VALUE,
				"negative values should return 'USUPPORTED_NEGATIVE_VALUE'");
	assert_true(parse_optarg_to_int("abc", &value) == UNSUPPORTED_VALUE,
				"unsupported alphanumeric string");
	assert_true(parse_optarg_to_int("42abc", &value) == UNSUPPORTED_VALUE,
				"unsupported alphanumeric suffix");
	assert_true(parse_optarg_to_int("abc42", &value) == UNSUPPORTED_VALUE,
				"unsupported alphanumeric prefix");
	assert_true(parse_optarg_to_int("", &value) == UNSUPPORTED_VALUE,
				"unsupported empty stringx");
	assert_true(parse_optarg_to_int(NULL, &value) == REQUIRED_ARG_NULL,
				"null string should return 'REQUIRED_ARG_NULL'");
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

int main(void)
{
	test_header_of_text();
	test_format_date();
	test_parse_optarg_to_int();
	test_escape_special_chars();
}
