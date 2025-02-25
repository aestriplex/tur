#ifndef __STR_H__
#define __STR_H__

#include <stdint.h>

typedef struct {
	const char*val;
	uint16_t len;
} str_t;

str_t str_init(const char *str, uint16_t len);
str_t str_init_unbounded(const char *str);

#endif /* __STR_H__ */ 
