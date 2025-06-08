/* test.h
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

#ifndef __TEST_H__
#define __TEST_H__

#include "../src/utils.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {
	PASS_ON_FAIL,
	EXIT_ON_FAIL,
} exit_mode_t;

void set_exit_mode(exit_mode_t exit_mode);
void print_report(void);

/* assertions */
void assert_true(bool condition, const char *message);

#endif /* __TEST_H__ */
