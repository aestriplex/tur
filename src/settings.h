#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "str.h"

#include <stdbool.h>

typedef enum {
	STDOUT = 0,
	LATEX,
	HTML,
	JEKYLL
} tur_output_t;

typedef struct {
	bool verbose;
	bool format_cache;
	str_t email;
	tur_output_t output;
	str_t repos_path;
} settings_t;

settings_t default_settings(void);

#endif /* __SETTINGS_H__ */
