/* loader.c - loads the commits, either from repositories or from cache
 * -----------------------------------------------------------------------
 * Copyright (C) 2025 - 2026 Matteo Nicoli
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

#include "loader.h"

#include "cache.h"
#include "commit.h"
#include "log.h"
#include "utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static return_code_t init_history(work_history_t **history)
{
    work_history_t *new_history = malloc(sizeof(work_history_t));
    if (!new_history) { return RUNTIME_MALLOC_ERROR; }

    commit_array_init(&new_history->commit_arr);
    new_history->authored_idx = NULL;
    new_history->co_authored_idx = NULL;
    new_history->n_authored = 0;
    new_history->n_co_authored = 0;
    new_history->tot_lines_added = 0;
    new_history->tot_lines_removed = 0;

    *history = new_history;
    return OK;
}

static return_code_t parse_cache_commit_line(commit_t *commit, char *line)
{
    char *cursor = line;
    char *field = NULL;
    char *fields[7] = { 0 };

    for (size_t i = 0; i < 7; i++) {
        field = strchr(cursor, '\t');
        if (!field) { return COMMITS_FILE_HASH_CORRUPTED; }
        *field = '\0';
        fields[i] = cursor;
        cursor = field + 1;
    }

    if (!fields[0] || strlen(fields[0]) == 0) {
        return COMMITS_FILE_HASH_CORRUPTED;
    }

    char resp_char = fields[1][0];
    if (resp_char != 'A' && resp_char != 'C') {
        return COMMITS_FILE_HASH_CORRUPTED;
    }

    char *endptr = NULL;
    long long raw_date = strtoll(fields[2], &endptr, 10);
    if (endptr == fields[2] || *endptr != '\0') {
        return COMMITS_FILE_HASH_CORRUPTED;
    }

    size_t files_changed = strtoull(fields[3], &endptr, 10);
    if (endptr == fields[3] || *endptr != '\0') {
        return COMMITS_FILE_HASH_CORRUPTED;
    }

    size_t lines_added = strtoull(fields[4], &endptr, 10);
    if (endptr == fields[4] || *endptr != '\0') {
        return COMMITS_FILE_HASH_CORRUPTED;
    }

    size_t lines_removed = strtoull(fields[5], &endptr, 10);
    if (endptr == fields[5] || *endptr != '\0') {
        return COMMITS_FILE_HASH_CORRUPTED;
    }

    int favorite = strtol(fields[6], &endptr, 10);
    if (endptr == fields[6] || *endptr != '\0') {
        return COMMITS_FILE_HASH_CORRUPTED;
    }

    *commit = (commit_t) {
        .hash = str_init(fields[0], strlen(fields[0])),
        .is_favorite = favorite != 0,
        .responsability = resp_char == 'A' ? AUTHORED : CO_AUTHORED,
        .date = (time_t)raw_date,
        .msg = str_init(cursor, strlen(cursor)),
        .stats = (commit_stats_t) {
            .files_changed = files_changed,
            .lines_added = lines_added,
            .lines_removed = lines_removed,
        },
    };

    return OK;
}

static str_t parse_cached_head(const char *line)
{
    const char *head_prefix = "\t@HEAD=";
    const char *head_ptr = strstr(line, head_prefix);
    if (!head_ptr) {
        return empty_str();
    }

    head_ptr += strlen(head_prefix);
    if (*head_ptr == '\0') {
        return empty_str();
    }

    return str_init(head_ptr, strlen(head_ptr));
}

return_code_t load_cached_history(const repository_t *repo,
                              work_history_t **history,
                              str_t *cached_head)
{
    if (!repo || !history) { return NULL_PARAMETER; }
    *history = NULL;
    if (cached_head) {
        *cached_head = empty_str();
    }

    if (!full_cache_file_exists()) { return OK; }

    FILE *fp = fopen(FULL_CACHE_FILE, "r");
    if (!fp) {
        (void)log_err("load_cached_history: cannot open `%s`\n", FULL_CACHE_FILE);
        return COMMIT_FILE_DOES_NOT_EXIST;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    unsigned current_repo_id = (unsigned)-1;
    bool in_target_repo = false;
    return_code_t ret = OK;

    while ((read = getline(&line, &len, fp)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[--read] = '\0';
        }

        if (line[0] == '+') {
            ret = parse_commit_id(&current_repo_id, line);
            if (ret != OK) { goto cleanup; }

            in_target_repo = current_repo_id == repo->id;
            if (in_target_repo && cached_head) {
                *cached_head = parse_cached_head(line);
            }
            if (!in_target_repo && *history) {
                break;
            }

            if (in_target_repo && !*history) {
                ret = init_history(history);
                if (ret != OK) { goto cleanup; }
            }

            continue;
        }

        if (!in_target_repo || !*history || strlen(line) == 0) {
            continue;
        }

        commit_t commit = { 0 };
        ret = parse_cache_commit_line(&commit, line);
        if (ret != OK) { goto cleanup; }

        ret = commit_array_add((*history)->commit_arr, &commit);
        if (ret != OK) {
            (void)log_err("load_cached_history: cannot add cached commit `%s`\n",
                          commit.hash.val);
            str_free(commit.hash);
            str_free(commit.msg);
            ret = RUNTIME_ARRAY_REALLOC_ERROR;
            goto cleanup;
        }

        if (commit.responsability == AUTHORED) {
            (*history)->n_authored++;
        } else {
            (*history)->n_co_authored++;
        }

        (*history)->tot_lines_added += commit.stats.lines_added;
        (*history)->tot_lines_removed += commit.stats.lines_removed;

        str_free(commit.hash);
        str_free(commit.msg);
    }

cleanup:
    if (ret != OK && *history) {
        history_free(history);
    }

    if (line) {
        free(line);
    }

    fclose(fp);
    return ret;
}
