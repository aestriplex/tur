/* utils.c
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
#include "commit.h"
#include "log.h"
#include "repo.h"
#include "str.h"
#include "utils.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_EDITOR "vi"
#define TUR_DIR ".tur/"

#define GITHUB_URL      "commit/"
#define GITHUB_URL_SIZE 7
#define GITLAB_URL      "-/commit/"
#define GITLAB_URL_SIZE 9

static str_t time_to_full_string(time_t timestamp)
{
	const char *date = ctime(&timestamp);
	return str_init(date, (uint16_t) strlen(date) - 1);
}

static str_t time_to_date_string(time_t timestamp)
{
	struct tm tm_info;
	static char buffer[DATE_PATTERN_SIZE];

	gmtime_r(&timestamp, &tm_info);
	strftime(buffer, sizeof(buffer), DATE_PATTERN, &tm_info);

	return str_init(buffer, strlen(buffer));
}

str_t format_date(time_t timestamp, bool date_only)
{
	return date_only
		   ? time_to_date_string(timestamp)
		   : time_to_full_string(timestamp);
}

static str_t get_commit_url(str_t repo_url, str_t commit_hash, str_t provider_url)
{
	/* Adding one for the NULL terminator */
	const size_t new_len = repo_url.len + provider_url.len + GIT_HASH_LEN + 1;
	char *url = malloc(new_len * sizeof(char));
	snprintf(url, new_len, "%s%s%s", repo_url.val, provider_url.val, commit_hash.val);

	return str_init(url, new_len);
}

str_t get_github_commit_url(str_t repo_url, str_t commit_hash)
{
	return get_commit_url(repo_url, commit_hash, str_init(GITHUB_URL, GITHUB_URL_SIZE));
}

str_t get_gitlab_commit_url(str_t repo_url, str_t commit_hash)
{
	return get_commit_url(repo_url, commit_hash, str_init(GITLAB_URL, GITLAB_URL_SIZE));
}

str_t get_raw_url(str_t repo_url, str_t commit_hash)
{
	return get_commit_url(repo_url, commit_hash, empty_str());
}

str_t get_first_line(str_t input)
{
	char output[4096];

	if (input.len == 0) { return empty_str(); }
	
	size_t len = strcspn(input.val, "\n");
	if (len >= input.len) {
		len = input.len;
	}
	
	memcpy(output, input.val, len);
	output[len] = '\0';

	return str_init(output, len);
}

char* trim_whitespace(const char *str)
{
	while (isspace((unsigned char)*str)) str++;
	
	if (*str == '\0') { return strdup(""); }
	
	const char *end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end)) end--;
	
	size_t len = end - str + 1;
	char *trimmed = (char*)malloc(len + 1);
	if (!trimmed) { return NULL; }
	
	strncpy(trimmed, str, len);
	trimmed[len] = 0;
	
	return trimmed;
}

str_t escape_special_chars(str_t input)
{
	uint16_t extra_chars = 0;
	for (uint16_t i = 0; i < input.len; i++) {
		if (input.val[i] == '_' || input.val[i] == '#') {
			extra_chars++;
		}
	}
	
	uint16_t new_len = input.len + extra_chars;
	char *escaped_str = malloc(new_len + 1);
	if (!escaped_str) {
		(void)log_err("escape_special_chars: memory allocation failed\n");
		return empty_str();
	}
	
	uint16_t j = 0;
	for (uint16_t i = 0; i < input.len; i++) {
		if (input.val[i] == '_' || input.val[i] == '#') {
			escaped_str[j++] = '\\';
		}
		escaped_str[j++] = input.val[i];
	}
	escaped_str[j] = '\0';
	
	str_t escaped_string = str_init(escaped_str, new_len);
	free(escaped_str);

	return escaped_string;
}

str_t get_editor_or_default(void)
{
	/* We use the same editor that has been chosen for git. */
	const char* editor = getenv("GIT_EDITOR");
	if (!editor) editor = DEFAULT_EDITOR;
	return str_init(editor, strnlen(editor, 20));
}

return_code_t check_or_create_tur_dir(void)
{
	struct stat st = { 0 };

	if (stat(TUR_DIR, &st) == -1) {
		if (mkdir(TUR_DIR, 0700) != 0) {
			(void)log_err("Cannot create the directory `%s`\n", TUR_DIR);
			return CANNOT_CREATE_TUR_DIR;
		}
	}

	return OK;
}
