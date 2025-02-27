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
#include <time.h>
#ifdef __linux__
#include <linux/time.h>
#endif

void assert_true(bool condition, const char *label)
{
	if (condition) { 
		printf(GREEN "PASSED (" TICK "): %s" RESET "\n", label); 
		return;
	}

	printf(RED "FAILED (" CROSS "): %s\n" RESET, label);
	exit(1);
}

uint64_t get_nanos(void)
{
	tick_t start;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	return (uint64_t) start.tv_sec*1e9 + (uint64_t) start.tv_nsec;
}

double get_time_diff(uint64_t prev_tick)
{
	return (get_nanos() - prev_tick)*1e-9;
}
