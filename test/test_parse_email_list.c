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
		str_t *result = parse_emails(test_input, &count);

		assert_true(count == 2, "Count of emails should be 2");
		assert_true(result[0].len == 18 && str_arr_equals(result[0], "email1@example.com"),
					"First email is 'email1@example.com'");
		assert_true(result[1].len == 18 && str_arr_equals(result[1], "email2@example.com"),
					"Second email is 'email2@example.com'");

		str_free(result[0]);
		str_free(result[1]);
		free(result);
	}

	{
		char *test_input = "single.email@example.com";
		str_t *result = parse_emails(test_input, &count);

		assert_true(count == 1, "Count of emails should be 1");
		assert_true(result[0].len == 24 && str_arr_equals(result[0], "single.email@example.com"),
					"Email 'single.email@example.com'");
		
		str_free(result[0]);
		free(result);
	}

	{
		char *test_input = "";
		str_t *result = parse_emails(test_input, &count);
		assert_true(count == 0, "Count of empty list should be 0");
		(void)result;
	}

	{
		char *test_input = "     ";
		str_t *result = parse_emails(test_input, &count);
		assert_true(count == 0, "Count of empty list with spaces should be 0");
		(void)result;
	}

	{
		char *test_input = "email1@example.com,";
		str_t *result = parse_emails(test_input, &count);
		assert_true(count == 1, "Count of emails should be 1");
		assert_true(result[0].len == 18 && str_arr_equals(result[0], "email1@example.com"),
					"The emails should be 'email1@example.com'");

		str_free(result[0]);
		free(result);
	}

	{
		char test_input[1024];
		memset(test_input, 0, 1024);
		for (int i = 0; i < 50; i++) {
			strcat(test_input, "email");
			char email[20];
			sprintf(email, "%d@example.com", i);
			strcat(test_input, email);
			if (i < 49) {
				strcat(test_input, ",");
			}
		}

		str_t *result = parse_emails(test_input, &count);
		assert_true(count == 50, "Count of emails should be 50");
		bool condition = true;
		for (int i = 0; i < 50; i++) {
			char expected_email[20];
			sprintf(expected_email, "email%d@example.com", i);
			condition &= str_arr_equals(result[i], expected_email);
			free((char *)result[i].val);
		}
		assert_true(condition, "All the emails are correct");
		free(result);
	}
}

int main(void)
{
	test_parse_emails();
}
