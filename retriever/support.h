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

int retriever_init_common(struct file_system_info *);

int retrieve_common(struct file_system_info *fsinfo, struct stats *, int);

int repo_number(struct file_system_info *, struct stats *);

int retrieve_rdiff(char *, char *, char *);

#endif
