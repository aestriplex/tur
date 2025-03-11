/* test_opts_args.c
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
#include "../src/opts_args.h"
#include "../src/settings.h"
#include "../src/str.h"

#include <stdlib.h>
#include <string.h>

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

void test_parse_sort_order(void) {
	{
		sort_ordering_t order;
		int result = parse_sort_order("ASC", 3, &order);
		assert_true(result == OK && order == ASC, "'ASC' is a correct sort order");
	}

	{
		sort_ordering_t order;
		int result = parse_sort_order("  ASC", 5, &order);
		assert_true(result == OK && order == ASC, "'  ASC' should be left-trimmed");
	}

	{
		sort_ordering_t order;
		int result = parse_sort_order("ASC    ", 7, &order);
		assert_true(result == OK && order == ASC, "'ASC    ' should be right-trimmed");
	}

	{
		sort_ordering_t order;
		int result = parse_sort_order("asc", 3, &order);
		assert_true(result == OK && order == ASC, "'asc' should be a correct sort order");
	}

	{
		sort_ordering_t order;
		int result = parse_sort_order("  asc", 5, &order);
		assert_true(result == OK && order == ASC, "'  asc' should be left-trimmed");
	}

	{
		sort_ordering_t order;
		int result = parse_sort_order("asc    ", 7, &order);
		assert_true(result == OK && order == ASC, "'asc    ' should be right-trimmed");
	}

	{
		sort_ordering_t order;
		int result = parse_sort_order("DESC", 4, &order);
		assert_true(result == OK && order == DESC, "'DESC' should be a correct sort order");
	}

	{
		sort_ordering_t order;
		int result = parse_sort_order("       DESC", 11, &order);
		assert_true(result == OK && order == DESC, "'       DESC' should be left-trimmed");
	}

	{
		sort_ordering_t order;
		int result = parse_sort_order("DESC      ", 11, &order);
		assert_true(result == OK && order == DESC, "'DESC      ' should be right-trimmed");
	}

	{
		sort_ordering_t order;
		int result = parse_sort_order("desc", 4, &order);
		assert_true(result == OK && order == DESC, "'desc' is a correct sort order");
	}

	{
		sort_ordering_t order;
		int result = parse_sort_order("      desc", 10, &order);
		assert_true(result == OK && order == DESC, "'      desc' should be left-trimmed");
	}
	
	{
		sort_ordering_t order;
		int result = parse_sort_order("desc       ", 11, &order);
		assert_true(result == OK && order == DESC, "'desc       ' should be right-trimmed");
	}

	{
		sort_ordering_t order;
		int result = parse_sort_order("qwerty", 6, &order);
		assert_true(result == UNKONWN_SORT_ORDER,
					"'qwerty' should be an unknown sort order");
	}

	{
		sort_ordering_t order;
		int result = parse_sort_order(NULL, 0, &order);
		assert_true(result == NULL_PARAMETER, "opt_str should not be NULL");
	}

	{
		int result = parse_sort_order("ASC", 0, NULL);
		assert_true(result == NULL_PARAMETER, "oreder should not be null");
	}

	{
		sort_ordering_t order;
		int result = parse_sort_order("", 0, &order);
		assert_true(result == OK && order == ASC, "'' should be an ascending order");
	}
}

int main(void)
{
	test_parse_optarg_to_int();
	test_parse_sort_order();
}
