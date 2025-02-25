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
