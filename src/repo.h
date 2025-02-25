#ifndef __REPO_H__
#define __REPO_H__

#include "settings.h"

#include <stdint.h>

#define AUTHORED 0x00
#define CO_AUTHORED 0x02

typedef struct {
	char *hash;
	uint8_t type;	
} commit_t;

typedef struct _commit_history {
	commit_t commit;
	struct _commit_history *parent;
} commit_history_t;

commit_history_t *get_commit_history(settings_t settings);

#endif /* __REPO_H__ */
