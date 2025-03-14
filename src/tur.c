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

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

static settings_t settings;
static struct option long_options[] = {
	{ "help",      no_argument,       0, 'h' },
	{ "diffs",     no_argument,       0, 'd' },
	{ "group",     no_argument,       0, 'g' },
	{ "version",   no_argument,       0, 'v' },
	{ "message",   no_argument,       0, 'm' },
	{ "date-only", no_argument,       0,  1  },
	{ "emails",    required_argument, 0, 'e' },
	{ "out",       required_argument, 0, 'o' },
	{ "repos",     required_argument, 0, 'r' },
	{ "sort",      required_argument, 0, 's' },
	{ 0, 0, 0, 0 }
};

static void print_help(void)
{
	printf("T U R\n"
		   "---------------------------------\n"
		   "Developed by aestriplex, (c) 2025\n"
		   "version %s\n"
		   "\n"
		   "Usage: tur [OPTIONS]\n"
		   "Options:\n"
		   "  -h, --help             Show this help message and exit\n"
		   "  -d, --diffs            Show diffs stats (rows added and removed, file changed)\n"
		   "  -g, --group            Group commit by repository\n"
		   "                         Default: false\n"
		   "  -m, --message          Shows the first line of the commit message\n"
		   "  -v, --version          Prints the verison on tur\n"
		   "                         Default: false\n"
		   "  --date-only            Each commit will be printed without time information\n"
		   "                         Format: Dec 28, 1994\n"
		   "  -e, --emails <e_1,...> Specify a list of email addresses\n"
		   "                         This list expects the emails separated by a comma."
		   "  -o, --out FILE         Specify an output file. Allowed extensions are:\n"
		   "                             .tex          (LaTeX)\n"
		   "                             .html / .html (HTML)\n"
		   "                             .md           (Markdown)\n"
		   "                         Default: stdout\n"
		   "  -r, --repos REPOS      Specify the file containing the list of repositories.\n"
		   "                         Default: .rlist in the current folder\n"
		   "  -s, --sort <order>     Sort commit by date.\n"
		   "                         Order:\n"
		   "                             ASC Ascending order;\n"
		   "                             DESC Descending order.\n"
		   "                         Default: false.\n"
		   "\n"
		   "Examples:\n"
		   "  tur -e user@example.com\n"
		   "  tur -e user1@example.com,user2@example.com -o commits.tex\n"
		   "  tur -dgs -e user1@example.com,user2@example.com -o commits.html\n"
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

	while ((ch = getopt_long(argc, argv, "hdgmve:o:r:s:", long_options, &option_index)) != -1) {
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
		case 'm':
			settings.print_msg = true;
			break;
		case 'v':
			printf("TUR version %s\n", __TUR_VERSION__);
			goto end;
		case 1:
			settings.date_only = true;
			break;
		case 'e':
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
		case 's':
			settings.sorted = true;
			ret = parse_sort_order(optarg, strlen(optarg), &settings.sort_order); 
			if (ret != OK) {
				fprintf(stderr, "Unknown sort order '%s'. Set deafult: ASC\n", optarg);
			}
			break;
		default:
			print_help();
			return -1;
		}
	}

	ret = get_repos_array(settings, &repos);
	if (ret != OK) { return ret; }

	ret = walk_through_repos(&repos, &settings);
	if (ret != OK) { return ret; }

end:
	return 0;
}
