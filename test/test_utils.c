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

		assert_true(result.len == 10, "mm/dd/yyyy has 10 characters");  // "MM/DD/YYYY" ha 10 caratteri
		assert_true(result.val[2] == '/', "there should be 2 chars for month");
		assert_true(result.val[5] == '/', "there should be 2 chars for day");
	}

	{
		struct tm tm_info = {0};
		tm_info.tm_year = 120;
		tm_info.tm_mon = 0;
		tm_info.tm_mday = 1;
		time_t timestamp = mktime(&tm_info);
		str_t result = format_date(timestamp);
		printf("%s\n", result.val);
		fflush(stdout);
		assert_true(str_arr_equals(result, "01/01/2020"), "date should be '01/01/2020'");
	}

	// // Test 3: Data con giorno e mese a un solo carattere - 9 gennaio 2020
	// {
	//     struct tm tm_info = {0};
	//     tm_info.tm_year = 120; // 2020 - 1900
	//     tm_info.tm_mon = 0;    // Gennaio
	//     tm_info.tm_mday = 9;   // 9 gennaio
	//     time_t timestamp = mktime(&tm_info);

	//     str_t result = format_date(timestamp);

	//     assert(result.len == 10);
	//     assert(strcmp(result.val, "01/09/2020") == 0);
	//     printf("Test 3 passato\n");
	// }

	// // Test 4: Una data futura - 25 dicembre 2099
	// {
	//     struct tm tm_info = {0};
	//     tm_info.tm_year = 199; // 2099 - 1900
	//     tm_info.tm_mon = 11;   // Dicembre
	//     tm_info.tm_mday = 25;  // 25 dicembre
	//     time_t timestamp = mktime(&tm_info);

	//     str_t result = format_date(timestamp);

	//     assert(result.len == 10);
	//     assert(strcmp(result.val, "12/25/2099") == 0);
	//     printf("Test 4 passato\n");
	// }

	// // Test 5: Data con anno a 2 cifre - 1 gennaio 1999
	// {
	//     struct tm tm_info = {0};
	//     tm_info.tm_year = 99;  // 1999 - 1900
	//     tm_info.tm_mon = 0;    // Gennaio
	//     tm_info.tm_mday = 1;   // Primo giorno
	//     time_t timestamp = mktime(&tm_info);

	//     str_t result = format_date(timestamp);

	//     assert(result.len == 10);
	//     assert(strcmp(result.val, "01/01/1999") == 0);
	//     printf("Test 5 passato\n");
	// }

	// // Test 6: Data con ora legale (inverno) - 1 novembre 2025
	// {
	//     struct tm tm_info = {0};
	//     tm_info.tm_year = 125; // 2025 - 1900
	//     tm_info.tm_mon = 10;   // Novembre
	//     tm_info.tm_mday = 1;   // 1 novembre
	//     time_t timestamp = mktime(&tm_info);

	//     str_t result = format_date(timestamp);

	//     assert(result.len == 10);
	//     assert(strcmp(result.val, "11/01/2025") == 0);
	//     printf("Test 6 passato\n");
	// }

	// // Test 7: Data con anno bisestile - 29 febbraio 2024
	// {
	//     struct tm tm_info = {0};
	//     tm_info.tm_year = 124; // 2024 - 1900
	//     tm_info.tm_mon = 1;    // Febbraio
	//     tm_info.tm_mday = 29;  // 29 febbraio
	//     time_t timestamp = mktime(&tm_info);

	//     str_t result = format_date(timestamp);

	//     assert(result.len == 10);
	//     assert(strcmp(result.val, "02/29/2024") == 0);
	//     printf("Test 7 passato\n");
	// }
}

int main(void)
{
	test_header_of_text();
	test_format_date();
}
