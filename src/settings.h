/* settings.h
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

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "str.h"

#include <stdbool.h>

typedef enum {
	STDOUT = 0,
	LATEX,
	HTML,
	JEKYLL
} tur_output_t;

typedef struct {
	bool format_cache;
	bool grouped;
	bool sorted;
	str_t *emails;
	int n_emails;
	tur_output_t output_mode;
	str_t output;
	str_t repos_path;
} settings_t;

settings_t default_settings(void);

#endif /* __SETTINGS_H__ */
