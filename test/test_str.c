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
#include "../src/str.h"

void test_str(void)
{
	{
		str_t s1 = str_init("Hello", 5);
		assert_true(str_arr_equals(s1, "Hello"), "str_init failed");
		str_t s2 = str_init("", 0);
		assert_true(str_arr_equals(s2, ""), "str_init failed for empty string");
		str_free(s1);
		str_free(s2);
	}

	{
		str_t s1 = str_init("Hello", 5);
		str_t s2 = str_init("Hello", 5);
		str_t s3 = str_init("World", 5);
		str_t s4 = str_init("Hello!", 6);
		assert_true(str_equals(s1, s2), "str_equals failed for equal strings");
		assert_true(!str_equals(s1, s3), "str_equals failed for different strings");
		assert_true(!str_equals(s1, s4), "str_equals failed for different lengths");
		str_free(s1);
		str_free(s2);
		str_free(s3);
		str_free(s4);
	}

	{
		str_t s1 = str_init("Hello", 5);
		str_t s2 = str_init(" World", 6);
		str_t s3 = str_concat(s1, s2);
		assert_true(str_arr_equals(s3, "Hello World"), "str_concat failed");
		str_t empty = str_init("", 0);
		str_t s4 = str_concat(s1, empty);
		assert_true(str_arr_equals(s4, "Hello"), "str_concat failed with empty string");
		str_free(s1);
		str_free(s2);
		str_free(s3);
		str_free(s4);
		str_free(empty);
	}

	{
		str_t s = str_init("Test", 4);
		assert_true(get_char(s, 0) == 'T', "get_char failed at index 0");
		assert_true(get_char(s, 3) == 't', "get_char failed at index 3");
		str_free(s);
	}

	{
		str_t empty = empty_str();
		assert_true(empty.len == 0, "Empty string length incorrect");
		assert_true(empty.val[0] == '\0', "Empty string value incorrect");
		str_free(empty);
	}

	{
		str_t s = str_init("The first factor is computed once", 33);
		char *cs_sub1 = "Th";
		char *cs_sub2 = " once";
		char *cs_sub3 = "r i";
		char *cs_sub4 = "e fe";
		char *cs_empty = "";
		str_t s_sub1 = str_init(cs_sub1, 2);
		str_t s_sub2 = str_init(cs_sub2, 5);
		str_t s_sub3 = str_init(cs_sub3, 3);
		str_t s_sub4 = str_init(cs_sub4, 4);
		str_t empty = empty_str();

		assert_true(str_contains_chars(s, cs_sub1), "string (char *) should contain 'Th' substring");
		assert_true(str_contains_chars(s, cs_sub2), "string (char *) should contain ' once' substring");
		assert_true(str_contains_chars(s, cs_sub3), "string (char *) should contain 'r i' substring");
		assert_true(str_contains_chars(s, cs_empty), "string (char *) should contain the empty string");
		assert_true(!str_contains_chars(s, cs_sub4), "string (char *) should NOT contain 'e fe' substring");
		
		assert_true(str_contains(s, s_sub1), "string (str_t) should contain 'Th' substring");
		assert_true(str_contains(s, s_sub2), "string (str_t) should contain ' once' substring");
		assert_true(str_contains(s, s_sub3), "string (str_t) should contain 'r i' substring");
		assert_true(str_contains(s, empty), "string (str_t) should contain the empty string");
		assert_true(!str_contains(s, s_sub4), "string (str_t) should NOT contain 'e fe' substring");

		str_free(s);
		str_free(s_sub1);
		str_free(s_sub2);
		str_free(s_sub3);
		str_free(s_sub4);
		str_free(empty);
	}
}

int main(void)
{
	test_str();
}
