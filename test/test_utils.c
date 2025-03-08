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
#include "../src/str.h"

#include <string.h>
 
void test_header_of_text(void)
{
	{
		const char *text = "First row\nSecond row\nThird row";
		str_t result = get_first_line(text, strlen(text));

		assert_true(result.len == 9, "first line of text should have length 10");
		assert_true(str_arr_equals(result, "First row"), "first line is 'First row'");
	}

	{
		const char *text = "A row without a newline";
		str_t result = get_first_line(text, strlen(text));
		assert_true(result.len == strlen(text),
					"the first line has the same length of to the text itself");
		assert_true(str_arr_equals(result, text),
					"the first line is equal to the text itself");
	}

	{
		const char *text = "";
		str_t result = get_first_line(text, strlen(text));

		assert_true(result.len == 0, "empty text has length 0");
		assert_true(str_equals(result, empty_str()),
					"empty text is equal to the empty string");
	}

	{
		const char *text = "\nSecond row";
		str_t result = get_first_line(text, strlen(text));

		assert_true(result.len == 0, "the first line should have length 0");
		assert_true(str_equals(result, empty_str()),
					"the first line is the empty string");
	}

	{
		str_t result = get_first_line(NULL, 10);

		assert_true(result.len == 0, "NULL text should produce a string of length 0");
		assert_true(str_equals(result, empty_str()),
					"NULL text should produce the empty string");
	}

	{
		const char *text = "This is a test string";
		str_t result = get_first_line(text, 0);

		assert_true(result.len == 0,
					"If I copy 0 chars, I should get a string of length 0");
		assert_true(str_equals(result, empty_str()),
					"If I copy 0 chars, I should get the empty string");
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
	}

	{
		time_t timestamp = 1732838400;
		str_t result = format_date(timestamp);
		assert_true(result.len == 10, "11/29/2024 has 10 characters");
		assert_true(str_arr_equals(result, "11/29/2024"), "date should be '11/29/2024'");
	}

	{
		time_t timestamp = 1578528000;
		str_t result = format_date(timestamp);
		assert_true(result.len == 10, "01/09/2020 has 10 characters");
		assert_true(str_arr_equals(result, "01/09/2020"), "date should be '01/09/2020'");
	}

	{
		time_t timestamp = 4101840000;
		str_t result = format_date(timestamp);
		assert_true(result.len == 10, "12/25/2099 has 10 characters");
		assert_true(str_arr_equals(result, "12/25/2099"), "date should be '12/25/2099'");
	}

	{
		time_t timestamp = 1709164800;
		str_t result = format_date(timestamp);
		assert_true(result.len == 10, "02/29/2024 has 10 characters");
		assert_true(str_arr_equals(result, "02/29/2024"),
					"Leap year (2024), date should be '02/29/2024'");
	}
}

int main(void)
{
	test_header_of_text();
	test_format_date();
}
