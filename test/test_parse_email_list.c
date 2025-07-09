/* test_parse_email_list.c
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

#include "../src/opts_args.h"
#include "../src/str.h"
#include "test.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_parse_emails(void)
{
	size_t count;

	{
		char *test_input = "email1@example.com,email2@example.com";
		str_array_t *result = parse_emails(test_input);
		str_t email1 = str_array_get(result, 0);
		str_t email2 = str_array_get(result, 1);

		assert_true(result->len == 2, "Count of emails should be 2");
		assert_true(email1.len == 18 && str_arr_equals(email1, "email1@example.com"),
					"First email is 'email1@example.com'");
		assert_true(email2.len == 18 && str_arr_equals(email2, "email2@example.com"),
					"Second email is 'email2@example.com'");

		// str_free(email1);
		// str_free(email2);
		str_array_free(&result);
	}

	{
		char *test_input = "single.email@example.com";
		str_array_t *result = parse_emails(test_input);
		str_t email1 = str_array_get(result, 0);

		assert_true(result->len == 1, "Count of emails should be 1");
		assert_true(email1.len == 24 && str_arr_equals(email1, "single.email@example.com"),
					"Email 'single.email@example.com'");
		
		str_array_free(&result);
	}

	{
		char *test_input = "";
		str_array_t *result = parse_emails(test_input);

		assert_true(result->len == 0, "Count of empty list should be 0");

		str_array_free(&result);
	}

	{
		char *test_input = "     ";
		str_array_t *result = parse_emails(test_input);

		assert_true(result->len == 0, "Count of empty list with spaces should be 0");

		str_array_free(&result);
	}

	{
		char *test_input = "email1@example.com,";
		str_array_t *result = parse_emails(test_input);
		str_t email1 = str_array_get(result, 0);

		assert_true(result->len == 1, "Count of emails should be 1");
		assert_true(email1.len == 18 && str_arr_equals(email1, "email1@example.com"),
					"The emails should be 'email1@example.com'");

		str_array_free(&result);
	}

	{
		char test_input[1024];
		memset(test_input, 0, 1024);
		for (int i = 0; i < 50; i++) {
			strcat(test_input, "email");
			char email[20];
			snprintf(email, 20, "%d@example.com", i);
			strcat(test_input, email);
			if (i < 49) {
				strcat(test_input, ",");
			}
		}

		str_array_t *result = parse_emails(test_input);

		assert_true(result->len == 50, "Count of emails should be 50");

		bool condition = true;
		for (int i = 0; i < 50; i++) {
			char expected_email[20];
			str_t got_email = str_array_get(result, i);
			snprintf(expected_email, 20, "email%d@example.com", i);
			condition &= str_arr_equals(got_email, expected_email);
		}

		assert_true(condition, "All the emails are correct");

		str_array_free(&result);
	}
}

int main(void)
{
	test_parse_emails();
	print_report();
}
