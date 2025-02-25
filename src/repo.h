#ifndef __REPO_H__
#define __REPO_H__

#include "codes.h"
#include "settings.h"
#include "str.h"

#include <stdint.h>

#define AUTHORED 0x00
#define CO_AUTHORED 0x02

typedef struct {
	str_t url;
	str_t path;
} repository_t;

typedef struct _repository_list {
	repository_t repository;
	struct _repository_list *next;
} repository_list_t;

typedef struct {
	char *hash;
	uint8_t type;	
} commit_t;

typedef struct _commit_history {
	commit_t commit;
	struct _commit_history *parent;
} commit_history_t;

commit_history_t *get_commit_history(repository_t repo, settings_t settings);
return_code_t get_repos_list(settings_t settings, repository_list_t *list);

#endif /* __REPO_H__ */
