#include "opts_args.h"
#include "settings.h"

#include <string.h>

tur_output_t parse_output_file_ext(const char *arg)
{
	const char* dot = strrchr(arg, '.');
	
	if (!dot || dot == arg) { goto default_ret; }
	
	dot++;
	
	// Compare with known extensions and return corresponding enum value
	if (strcasecmp(dot, "tex") == 0) {
		return LATEX;
	} else if (strcasecmp(dot, "html") == 0 || strcasecmp(dot, "htm") == 0) {
		return HTML;
	} else if (strcasecmp(dot, "md") == 0) {
		return JEKYLL;
	}

default_ret:
	return STDOUT;
}
