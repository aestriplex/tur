/* view.h
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

#ifndef __VIEW_H__
#define __VIEW_H__

#include "repo.h"
#include "settings.h"

void generate_latex_file(const repository_array_t *repos, settings_t *settings);
void print_stdout(const repository_array_t *repos, settings_t *settings);
void generate_html_file(const repository_array_t *repos, settings_t *settings);
void generate_markdown_file(const repository_array_t *repos, settings_t *settings);

#endif /* __VIEW_H__ */
