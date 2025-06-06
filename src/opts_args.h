/* opts_args.h
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

#ifndef __OPTS_ARGS__
#define __OPTS_ARGS__

#include "settings.h"
#include "str.h"

tur_output_t parse_output_file_ext(const char *arg);
str_t *parse_emails(const char *input, size_t *count);
uint16_t parse_optarg_to_int(const char *optarg, unsigned *out_value);
uint16_t parse_sort_order(const char *opt_str, size_t len, sort_ordering_t *order);

#endif /* __OPTS_ARGS__ */
