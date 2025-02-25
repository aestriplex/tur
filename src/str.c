#include "str.h"

#include <stdint.h>
#include <string.h>

str_t str_init(const char *str, uint16_t len)
{
	return (str_t) {
		.val = str,
		.len = len
	};
}

/* this should be used only if it's guaranteed that str is NULL terminating */
str_t str_init_unbounded(const char *str)
{
	return (str_t) {
		.val = str,
		.len = strnlen(str, UINT16_MAX)
	};
}
