/* stdout.c
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

#include "utils.h"
#include "view.h"

#include <stdio.h>

void print_stdout(const repository_array_t *repos, settings_t settings)
{
	for (size_t i = 0; i < repos->count; i++) {
		const repository_t repo = repos->repositories[i];
		const commit_refs_t *authored = repo.history->authored;
		const commit_refs_t *co_authored = repo.history->co_authored;

		fprintf(stdout, "Repository: %s\n", repo.name.val);

		if(repo.history->n_authored == 0) { goto co_authored; }

		fprintf(stdout, "Authored commits:\n");
		for (size_t n_c = 0; n_c < repo.history->n_authored; n_c++) {
			fprintf(stdout, "\t%s %s",
					authored->commits[n_c]->hash.val,
					time_to_string(authored->commits[n_c]->date).val);
		}

	co_authored:

		if(repo.history->n_co_authored == 0) { continue; }

		fprintf(stdout, "Co-authored commits:\n");
		for (size_t n_c = 0; n_c < repo.history->n_co_authored; n_c++) {
			fprintf(stdout, "\t%s %s",
					co_authored->commits[n_c]->hash.val,
					time_to_string(co_authored->commits[n_c]->date).val);
		}
	}

	fflush(stdout);
}
