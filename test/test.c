/* test.c
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

#include <stdlib.h>

typedef void(*on_fail_fn)(void);

static void exit_on_fail(void) { exit(1); }
static void pass_on_fail(void) { ; }
static unsigned long passed = 0;
static unsigned long failed = 0;
static on_fail_fn on_fail = exit_on_fail;

void assert_true(bool condition, const char *label)
{
	if (condition) { 
		printf(GREEN "PASSED (" TICK "): %s" RESET "\n", label);
		passed++;
		return;
	}

	printf(RED "FAILED (" CROSS "): %s\n" RESET, label);
	failed++;
	on_fail();
}

void set_exit_mode(exit_mode_t exit_mode)
{
	if (exit_mode == PASS_ON_FAIL) {
		on_fail = pass_on_fail;
	} else {
		on_fail = exit_on_fail;
	}
}

void print_report(void)
{
	printf("%s======================\n"
		   "PASSED: %lu\nFAILED: %lu\n"
		   "======================" RESET "\n",
		   failed == 0 ? GREEN : RED, passed, failed);
}
