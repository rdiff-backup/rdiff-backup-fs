#ifndef _RETRIEVER_SIMPLE_H_
#define _RETRIEVER_SIMPLE_H_

#include <pthread.h>
#include "../headers.h"
#include "../externs.h"
#include "../support/gstring.h"
#include "../support/gpath.h"
#include "../structure/core.h"

int retrieve_simple(struct file_system_info *fsinfo, struct stats *);

int release_simple(struct file_system_info *fsinfo, struct stats *);

#endif
