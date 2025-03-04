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
	}

	{
		str_t s = str_init("Test", 4);
		assert_true(get_char(s, 0) == 'T', "get_char failed at index 0");
		assert_true(get_char(s, 3) == 't', "get_char failed at index 3");
		str_free(s);
	}

	{
		str_t empty = str_init("", 0);
		assert_true(empty.len == 0, "Empty string length incorrect");
		assert_true(empty.val[0] == '\0', "Empty string value incorrect");
		str_free(empty);
	}
}

int main(void)
{
	test_str();
}
