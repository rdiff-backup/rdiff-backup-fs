#ifndef _RETRIEVER_SUPPORT_H_
#define _RETRIEVER_SUPPORT_H_

#include <pthread.h>
#include "../headers.h"
#include "../externs.h"
#include "../errors.h"
#include "../support/gstring.h"
#include "../support/gpath.h"
#include "../structure/core.h"

#define lock(mutex) pthread_mutex_lock(&mutex)
#define unlock(mutex) pthread_mutex_unlock(&mutex)

extern pthread_mutex_t **file_mutex;

int retrieve_common(struct stats *, int);

int repo_number(struct stats *);

int retrieve_rdiff(char *, char *, char *);

int create_tmp_file(struct stats *);

#endif
