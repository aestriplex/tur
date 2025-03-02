/* opts_args.c
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

#include "opts_args.h"
#include "settings.h"
#include "str.h"
#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_EMAIL_ARR_SIZE 10

tur_output_t parse_output_file_ext(const char *arg)
{
	const char* dot = strrchr(arg, '.');
	
	if (!dot || dot == arg) { goto default_ret; }
	
	dot++;
	
	if (strcasecmp(dot, "tex") == 0) {
		return LATEX;
	} else if (strcasecmp(dot, "html") == 0 || strcasecmp(dot, "htm") == 0) {
		return HTML;
	} else if (strcasecmp(dot, "md") == 0) {
		return JEKYLL;
	}

default_ret:
	fprintf(stderr, "Invalid file extension: .%s; output redirected on stdout.\n", dot);
	return STDOUT;
}

str_t *parse_emails(const char *input, int *count)
{
	size_t len = strlen(input);
	char *str = strdup(input);
	if (!str) {
		perror("malloc failed");
		return NULL;
	}

	str = trim_whitespace(str);

	if (len > 1 && str[0] == '[') str[0] = ' ';
	if (len > 1 && str[len - 1] == ']') str[len - 1] = '\0';

	char *trimmed_str = trim_whitespace(str);

	if (strlen(trimmed_str) == 0) {
		*count = 0;
		free(str);
		return NULL;
	}

	size_t capacity = DEFAULT_EMAIL_ARR_SIZE;
	str_t *emails = malloc(capacity * sizeof(str_t));
	if (!emails) {
		perror("malloc failed");
		free(str);
		return NULL;
	}

	*count = 0;

	char *start = trimmed_str;
	char *end = NULL;

	while ((end = strstr(start, ",")) != NULL) {
		*end = '\0';

		char *trimmed_email = trim_whitespace(start);

		emails[*count].val = strdup(trimmed_email);
		if (!emails[*count].val) {
			perror("malloc failed");
			for (int i = 0; i < *count; i++) {
				free((char *)emails[i].val);
			}
			free(emails);
			free(str);
			return NULL;
		}
		emails[*count].len = strlen(emails[*count].val);

		(*count)++;

		if (*count >= capacity) {
			capacity += DEFAULT_EMAIL_ARR_SIZE;
			emails = realloc(emails, capacity * sizeof(str_t));
			if (!emails) {
				perror("realloc failed");
				for (int i = 0; i < *count; i++) {
					free((char *)emails[i].val);
				}
				free(str);
				return NULL;
			}
		}

		start = end + 1;
	}

	if (*start != '\0') {
		char *trimmed_email = trim_whitespace(start);

		emails[*count].val = strdup(trimmed_email);
		if (!emails[*count].val) {
			perror("malloc failed");
			for (int i = 0; i < *count; i++) {
				free((char *)emails[i].val);
			}
			free(emails);
			free(str);
			return NULL;
		}
		emails[*count].len = strlen(emails[*count].val);
		(*count)++;
	}

	free(str);
	return emails;
}

