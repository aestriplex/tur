#include "settings.h"
#include "str.h"

#include <stdlib.h>

static str_t default_repos_path;

settings_t default_settings(void)
{
	return (settings_t) {
		.output = STDOUT,
		.format_cache = false,
		.verbose = false,
		.repos_path = default_repos_path,
		.email = str_init_unbounded(0)
	};
}
