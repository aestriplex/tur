/* tur.c
 * -----------------------------------------------------------------------
 * Copyright (C) 2025  Matteo Nicoli
 *
 * This file is part of tur
 *
 * tur is free software; you can redistribute it and/or modify
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

#include "opts_args.h"
#include "repo.h"
#include "settings.h"
#include "str.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static settings_t settings;
static struct option long_options[] = {
	{ "help",    no_argument,       0, 'h' },
	{ "verbose", no_argument,       0, 'v' },
	{ "format",  no_argument,       0, 'f' },
	{ "email",   required_argument, 0, 'e' },
	{ "out",     required_argument, 0, 'o' },
	{ "repos",   required_argument, 0, 'r' },
	{ 0, 0, 0, 0 }
};

void print_help(void)
{
	printf("\n"
		   "T U R\n"
		   "---------------------------------\n"
		   "Developed by aestriplex, (c) 2025\n"
		   "\n"
		   "Usage:\ttur [h] [v] [o] [r]\n"
		   "\n");
}

int main(int argc, char *argv[]) {
	repository_list_t *head = NULL;
	return_code_t ret;
	int ch, option_index = 0;

	if (argc == 1) {
		print_help();
		return 0;
	}

	settings = default_settings();

	while ((ch = getopt_long(argc, argv, "hvfo:r:", long_options, &option_index)) != -1) {
		switch (ch) {
		case 'h':
			print_help();
			return 0;
		case 'v':
			settings.verbose = true;
			break;
		case 'f':
			settings.format_cache = true;
			break;
		case 'e':
			settings.email = str_init_unbounded(optarg);
			break;
		case 'o':
			settings.output = parse_output_file_ext(optarg);
			break;
		case 'r':
			settings.repos_path = str_init_unbounded(optarg);
			break;
		default:
			print_help();
			return -1;
		}
	}

	head = malloc(sizeof(repository_list_t));
	ret = get_repos_list(settings, head);

	if (ret != OK) { return ret; }

	
	
	return 0;
}
