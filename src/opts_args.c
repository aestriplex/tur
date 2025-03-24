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

#include "codes.h"
#include "opts_args.h"
#include "settings.h"
#include "str.h"
#include "utils.h"

#include <ctype.h>
#include <limits.h>
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
	char *trimmed_str = trim_whitespace(input);
	if (!trimmed_str) {
		fprintf(stderr, "[parse_emails] cannot duplicate input string: malloc error\n");
		return NULL;
	}

	if (strlen(trimmed_str) == 0) {
		/* happy path, empty list */
		*count = 0;
		free(trimmed_str);
		return NULL;
	}

	size_t capacity = DEFAULT_EMAIL_ARR_SIZE;
	str_t *emails = malloc(capacity * sizeof(str_t));
	if (!emails) {
		fprintf(stderr, "[parse_emails] cannot allocate email array memory: malloc error\n");
		emails = NULL;
		goto cleanup_and_exit;
	}

	*count = 0;

	char *start = trimmed_str;
	char *end = NULL;

	while ((end = strstr(start, ",")) != NULL) {
		*end = '\0';
		emails[*count].val = strdup(start);
		if (!emails[*count].val) {
			fprintf(stderr, "[parse_emails] cannot duplicate email #%d: strdup error\n", *count);
			for (int i = 0; i < *count; i++) {
				free((char *)emails[i].val);
			}
			free(emails);
			emails = NULL;
			goto cleanup_and_exit;
		}
		emails[*count].len = strlen(emails[*count].val);

		(*count)++;

		if (*count >= capacity) {
			capacity += DEFAULT_EMAIL_ARR_SIZE;
			emails = realloc(emails, capacity * sizeof(str_t));
			if (!emails) {
				fprintf(stderr, "[parse_emails] cannot resize email arrays #%d: realloc error\n", *count);
				for (int i = 0; i < *count; i++) {
					free((char *)emails[i].val);
				}
				emails = NULL;
				goto cleanup_and_exit;
			}
		}

		start = end + 1;
	}

	if (*start != '\0') {
		emails[*count].val = strdup(start);
		if (!emails[*count].val) {
			fprintf(stderr, "[parse_emails] cannot duplicate email #%d: strdup error\n", *count);
			for (int i = 0; i < *count; i++) {
				free((char *)emails[i].val);
			}
			free(emails);
			emails = NULL;
			goto cleanup_and_exit;
		}
		emails[*count].len = strlen(emails[*count].val);
		(*count)++;
	}

cleanup_and_exit:
	free(trimmed_str);
	return emails;
}


uint16_t parse_optarg_to_int(const char *optarg, unsigned *out_value)
{
	if (!optarg) { return REQUIRED_ARG_NULL; }

	char *endptr;
	long val = strtol(optarg, &endptr, 10);
	if (val > UINT_MAX) { return INT_OVERFLOW; }
	if (val < 0) { return USUPPORTED_NEGATIVE_VALUE; }
	if (endptr == optarg || *endptr != '\0') { return UNSUPPORTED_VALUE; }

	*out_value = (int)val;
	return OK;
}

uint16_t parse_sort_order(const char *opt_str, size_t len, sort_ordering_t *order)
{
	uint16_t ret;
	char buffer[5];

	if (!opt_str || !order) { return NULL_PARAMETER; }

	if (len == 0) {
		*order = ASC;
		return OK;
	}

	char *str = trim_whitespace(opt_str);
	if (!str) {
		fprintf(stderr, "[parse_sort_order] cannot trim input string: malloc error\n");
		return RUNTIME_MALLOC_ERROR;
	}
	size_t trimmed_len = strlen(str);
	
	if (trimmed_len > 4) { 
		ret = UNKONWN_SORT_ORDER;
		goto cleanup_and_exit;
	}
	
	for (size_t i = 0; i < trimmed_len; i++) {
		buffer[i] = (char)toupper((int)str[i]);
	}
	buffer[trimmed_len] = '\0';
	
	if (strncmp(buffer, "ASC", 3) == 0) {
		*order = ASC;
		ret = OK;
		goto cleanup_and_exit;
	}

	if (strncmp(buffer, "DESC", 4) == 0) {
		*order = DESC;
		ret = OK;
		goto cleanup_and_exit;
	}
	
	ret = UNKONWN_SORT_ORDER;

cleanup_and_exit:
	free(str);
	return ret;
}
