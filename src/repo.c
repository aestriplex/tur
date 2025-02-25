#include "codes.h"
#include "repo.h"
#include "settings.h"
#include "str.h"

#include <stdlib.h>
#include <stdio.h>
#include <git2.h>

static void print_commit_info(const git_commit *commit) {
	const git_signature *author = git_commit_author(commit);
	printf("Commit: %s\n", git_oid_tostr_s(git_commit_id(commit)));
	printf("Author: %s <%s>\n", author->name, author->email);
	printf("Date: %s", ctime((const time_t *) &author->when.time));
	printf("Message: %s\n\n", git_commit_message(commit));
}

const return_code_t get_repos_list(settings_t settings, str_t *list)
{
	size_t len = 0;
    ssize_t read;
	char * line = NULL;
	str_t current_line;
	FILE *repos_list = fopen(settings.repos_path.val, "r");

	if (!repos_list) { return INVALID_REPOS_LIST_PATH; }

	while ((read = getline(&line, &len, repos_list)) != -1) {
        current_line = str_init(line, read);
    }

    fclose(repos_list);

	return OK;
}

commit_history_t *get_commit_history(settings_t settings) {
	commit_history_t *history = NULL;
	git_repository *repo = NULL;
	git_revwalk *walker = NULL;
	git_oid oid;
	git_commit *commit = NULL;

	git_libgit2_init();

	if (git_repository_open(&repo, "/Users/teo/Documents/GitHub/stats") != 0) {
		fprintf(stderr, "Failed to open repository\n");
		goto ret;
	}

	if (git_revwalk_new(&walker, repo) != 0) {
		fprintf(stderr, "Failed to create revwalk\n");
		goto cleanup;
	}

	git_revwalk_push_head(walker);
	git_revwalk_sorting(walker, GIT_SORT_TIME);

	while (git_revwalk_next(&oid, walker) == 0) {
		if (git_commit_lookup(&commit, repo, &oid) == 0) {
			print_commit_info(commit);
			git_commit_free(commit);
		}
	}

	git_revwalk_free(walker);
	git_libgit2_shutdown();

cleanup:
	git_repository_free(repo);
ret:
	return history;
}
