#ifndef _RETRIEVER_LIMIT_H_
#define _RETRIEVER_LIMIT_H_

#include <pthread.h>
#include "../headers.h"
#include "../externs.h"
#include "../support/gstring.h"
#include "../support/gpath.h"
#include "../structure/core.h"
#include "support.h"

extern pthread_mutex_t **file_mutex;

struct cache {
	struct stats *stats;
	struct cache *next;
};

int retrieve_limit(struct file_system_info *fsinfo, struct stats *);
int release_limit(struct file_system_info *fsinfo, struct stats *);

#endif
