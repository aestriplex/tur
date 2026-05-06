/* cache.c
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

#include "cache.h"
#include "commit.h"
#include "codes.h"
#include "log.h"
#include "lookup_table.h"
#include "utils.h"

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FAVORITE_STR "[*]"

/* Just return the key as it is */
uint32_t id_hash(void *key)
{
    return *(uint32_t *)key;
}

static void print_commit_line(FILE *fp, const commit_t *commit)
{
    fprintf(fp,
            "%s\t%s%s%s\n",
            commit->hash.val,
            get_first_line(commit->msg).val,
            commit->is_favorite ? "\t" : "",
            commit->is_favorite ? FAVORITE_STR : "");
}

static char get_resp_char(responsability_t resp)
{
    return resp == AUTHORED ? 'A' : 'C';
}

static void print_cache_commit_line(FILE *fp, const commit_t *commit)
{
    fprintf(fp,
            "%s\t%c\t%lld\t%zu\t%zu\t%zu\t%d\t%s\n",
            commit->hash.val,
            get_resp_char(commit->responsability),
            (long long)commit->date,
            commit->stats.files_changed,
            commit->stats.lines_added,
            commit->stats.lines_removed,
            commit->is_favorite ? 1 : 0,
            get_first_line(commit->msg).val);
}

static return_code_t repo_index(repository_t *repo, const cacheidx_arr_t *commits)
{
    size_t authored_count = 0, co_authored_count = 0;
    work_history_t *history = repo->history;

    for (size_t i = 0; i < history->commit_arr->len; i++) {
        commit_t *commit = commit_array_get(history->commit_arr, i);
        commit->is_favorite = false;
    }

    for (size_t i = 0; i < commits->len; i++) {
        cache_index_t *commit_idx = cache_array_get(commits, i);
        commit_t *c = get_commit_with_id(history->commit_arr, commit_idx->hash);
        if (!c) {
            (void)log_err("repo_index: cannot find commit `%s`\n",
                          commit_idx->hash.val);
            return COMMIT_NOT_FOUND;
        }

        c->is_favorite = commit_idx->is_favorite;

        if (c->responsability == AUTHORED) {
            history->authored_idx[authored_count++] = c;
        } else {
            history->co_authored_idx[co_authored_count++] = c;
        }
    }

    history->n_authored = authored_count;
    history->n_co_authored = co_authored_count;

    return OK;
}

static return_code_t parse_commit_file(table_t *repo_table)
{
    return_code_t ret = OK;
    FILE *fp = fopen(COMMITS_FILE, "r");
    if (!fp) {
        log_err("parse_commit_file: cannot open file `%s`\n", COMMITS_FILE);
        return COMMIT_FILE_DOES_NOT_EXIST;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    unsigned current_repo_id = (unsigned)-1;
    cacheidx_arr_t *current_commits = NULL;

    while ((read = getline(&line, &len, fp)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[--read] = '\0';
        }

        if (line[0] == '+') {
            if (current_commits) {
                table_put(repo_table, current_repo_id, current_commits);
                cache_array_free(&current_commits);
            }

            /* Parsing the repo id from this line:  "+ <ID>) <NAME>" */
            ret = parse_commit_id(&current_repo_id, line);
            if (ret != OK) { return ret; }

            cache_array_init(&current_commits);

        } else if (current_commits) {
            if (strlen(line) == 0) { continue; }

            char *end_of_hash = strchr(line, '\t');
            if (!end_of_hash) { return COMMITS_FILE_HASH_CORRUPTED; }
            
            size_t hash_len = (size_t)(end_of_hash - line);
            str_t hash = str_init(line, hash_len);

            /* If the commit name contains the string "[*]", then the commit
             * is labelled as a favorite.
             */
            bool is_favorite = strstr(end_of_hash, FAVORITE_STR);
            cache_index_t idx = (cache_index_t) {
                .hash = hash,
                .is_favorite = is_favorite,
            };

            ret = cache_array_add(current_commits, &idx);
            if (ret == RUNTIME_ARRAY_REALLOC_ERROR) {
                (void)log_err("parse_commit_file: cannot allocate enough "
                              "memory for the commit list in `%s`",
                               COMMITS_FILE);
                goto cleanup;
            }
        }
    }

    if (current_commits) {
        table_put(repo_table, current_repo_id, current_commits);
        cache_array_free(&current_commits);
    }

cleanup:
    free(line);
    fclose(fp);

    return ret;
}

return_code_t check_or_create_tur_dir(void)
{
    struct stat st = { 0 };

    if (stat(TUR_DIR, &st) == -1) {
        if (mkdir(TUR_DIR, 0700) != 0) {
            (void)log_err("Cannot create the directory `%s` "
                          "with permission 700\n", TUR_DIR);
            return CANNOT_CREATE_TUR_DIR;
        }
    }

    return OK;
}

bool commit_file_exists(void)
{
    struct stat st = { 0 };
    return stat(COMMITS_FILE, &st) != -1;
}

bool full_cache_file_exists(void)
{
    struct stat st = { 0 };
    return stat(FULL_CACHE_FILE, &st) != -1;
}

return_code_t write_repos_on_file(const repository_array_t *repos)
{
    return_code_t ret = OK;
    FILE *fp = NULL;

    ret = check_or_create_tur_dir();
    if (ret != OK) {
        return ret;
    }

    fp = fopen(COMMITS_FILE, "w");
    if (!fp) {
        (void)log_err("Cannot create file `%s`...\n", COMMITS_FILE);
        return CANNOT_CREATE_COMMITS_FILE;
    }

    for (size_t i = 0; i < repos->len; i++) {
        repository_t *repo = repo_array_get(repos, i);
        const work_history_t *history = repo->history;
        commit_t **const authored = history->authored_idx;
        commit_t **const co_authored = history->co_authored_idx;

        fprintf(fp, "+ %u) %s\n", repo->id, repo->name.val);
        for (size_t j = 0; j < history->n_authored; j++) {
            print_commit_line(fp, authored[j]);
        }
        for (size_t j = 0; j < history->n_co_authored; j++) {
            print_commit_line(fp, co_authored[j]);
        }
    }

    fclose(fp);
    return ret;
}

return_code_t write_full_cache_on_file(const repository_array_t *repos)
{
    return_code_t ret = OK;
    FILE *fp = NULL;

    ret = check_or_create_tur_dir();
    if (ret != OK) {
        return ret;
    }

    fp = fopen(FULL_CACHE_FILE, "w");
    if (!fp) {
        (void)log_err("Cannot create file `%s`...\n", FULL_CACHE_FILE);
        return CANNOT_CREATE_COMMITS_FILE;
    }

    for (size_t i = 0; i < repos->len; i++) {
        repository_t *repo = repo_array_get(repos, i);
        const work_history_t *history = repo->history;
        const commit_arr_t *commit_arr = history->commit_arr;
        const char *branch_name = repo->branches
                                  ? str_array_get(repo->branches, 0).val
                                  : NULL;
        str_t branch_head = empty_str();

        ret = get_branch_head_hash(repo->path, branch_name, &branch_head);
        if (ret != OK) {
            fclose(fp);
            return ret;
        }

        fprintf(fp,
                "+ %u) %s\t@HEAD=%s\n",
                repo->id,
                repo->name.val,
                str_not_empty(branch_head) ? branch_head.val : "");
        str_free(branch_head);
        for (size_t j = 0; j < commit_arr->len; j++) {
            commit_t *commit = commit_array_get(commit_arr, j);
            print_cache_commit_line(fp, commit);
        }
    }

    fclose(fp);
    return ret;
}

return_code_t rebuild_indexes(const repository_array_t *repos)
{
    return_code_t ret = OK;
    table_t repo_table;

    if (!commit_file_exists()) {
        (void)log_err("rebuild_indexes: commit file does not exist\n");
        return COMMIT_FILE_DOES_NOT_EXIST;
    }

    table_init(&repo_table, 10, 10, id_hash);
    ret = parse_commit_file(&repo_table);
    if (ret != OK) { return ret; }

    for (size_t i = 0; i < repos->len; i++) {
        cacheidx_arr_t *commits = table_get(&repo_table, i);
        if (!commits) { continue; }
        repository_t *repo = repo_array_get(repos, i);
        ret = repo_index(repo, commits);
        if (ret != OK) { return ret; }
    }

    return ret;
}

return_code_t delete_cache(void)
{
    if (remove(TUR_DIR) != 0) {
        perror("Error deleting cache dir");
        return CANNOT_DELETE_TUR_DIR;
    }
    return OK;
}

return_code_t delete_commits_index(void)
{
    if (remove(COMMITS_FILE) != 0) {
        perror("Error deleting commits index file");
        return CANNOT_DELETE_COMMITS_FILE;
    }
    return OK;
}

return_code_t delete_full_cache(void)
{
    if (remove(FULL_CACHE_FILE) != 0) {
        perror("Error deleting full cache file");
        return CANNOT_DELETE_COMMITS_FILE;
    }
    return OK;
}
