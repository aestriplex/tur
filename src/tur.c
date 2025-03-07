/* tur.c
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

#include "codes.h"
#include "opts_args.h"
#include "repo.h"
#include "settings.h"
#include "str.h"
#include "utils.h"
#include "walk.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

static settings_t settings;
static struct option long_options[] = {
	{ "help",      no_argument,       0, 'h' },
	{ "diffs",      no_argument,       0, 'd' },
	{ "group",     no_argument,       0, 'g' },
	{ "sort",      no_argument,       0, 's' },
	{ "version",   no_argument,       0, 'v' },
	{ "email",     required_argument, 0, 'e' },
	{ "mail-list", required_argument, 0, 'm' },
	{ "out",       required_argument, 0, 'o' },
	{ "repos",     required_argument, 0, 'r' },
	{ 0, 0, 0, 0 }
};

void print_help(void)
{
	printf("T U R\n"
		   "---------------------------------\n"
		   "Developed by aestriplex, (c) 2025\n"
		   "version %s\n"
		   "\n"
		   "Usage: tur [OPTIONS]\n"
		   "Options:\n"
		   "  -h, --help              Show this help message and exit\n"
		   "  -d, --diffs             Show diffs stats (rows added and removed, file changed)\n"
		   "  -g, --group             Group commit by repository\n"
		   "                          Default: false\n"
		   "  -s, --sort              Sort commit by date\n"
		   "                          Default: false\n"
		   "  -v, --version           Prints the verison on tur\n"
		   "                          Default: false\n"
		   "  -e, --email EMAIL       Specify a single email address\n"
		   "  -m, --mail-list FILE    Specify a comma separated email addresses\n"
		   "  -o, --out FILE          Specify an output file. Allowed extensions are:\n"
		   "                              .tex          (LaTeX)\n"
		   "                              .html / .html (HTML)\n"
		   "                              .md           (Markdown)\n"
		   "                          Default: stdout\n"
		   "  -r, --repos REPOS       Specify the file containing the list of repositories.\n"
		   "                          Default: .rlist in the current folder\n"
		   "\n"
		   "Examples:\n"
		   "  tur -e user@example.com\n"
		   "  tur -m user1@example.com,user2@example.com -o commits.tex\n"
		   "\n"
		   "\n",
		   __TUR_VERSION__);
}

int main(int argc, char *argv[])
{
	repository_array_t repos;
	return_code_t ret;
	int ch, n_emails, option_index = 0;

	if (argc == 1) {
		print_help();
		goto end;
	}

	settings = default_settings();

	while ((ch = getopt_long(argc, argv, "hdgsve:m:o:r:", long_options, &option_index)) != -1) {
		switch (ch) {
		case 'h':
			print_help();
			goto end;
		case 'd':
			settings.show_diffs = true;
			break;
		case 'g' :
			settings.grouped = true;
			break;
		case 's':
			settings.sorted = true;
			break;
		case 'v':
			printf("TUR version %s\n", __TUR_VERSION__);
			goto end;
		case 'e':
			settings.emails = malloc(sizeof(str_t));
			*(settings.emails)  = str_init(optarg, (uint16_t) strlen(optarg));
			settings.n_emails = 1;
			break;
		case 'm':
			settings.emails = parse_emails(optarg, &n_emails);
			settings.n_emails = n_emails;
			break;
		case 'o':
			settings.output_mode = parse_output_file_ext(optarg);
			settings.output = str_init(optarg, (uint16_t) strlen(optarg));
			break;
		case 'r':
			settings.repos_path = str_init(optarg, (uint16_t) strlen(optarg));
			break;
		default:
			print_help();
			return -1;
		}
	}

	ret = get_repos_array(settings, &repos);
	if (ret != OK) { return ret; }

	ret = walk_through_repos(&repos, settings);
	if (ret != OK) { return ret; }

end:
	return 0;
}
