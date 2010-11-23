#ifndef _LAYOUT_CORE_H_
#define _LAYOUT_CORE_H_

#include "../headers.h"
#include "../support/gstats.h"

extern int (*init)(char *repo);

extern int (*init_multi)(int count, char **repos);

extern int (*get_file)(const char *path, struct stats **stats);

extern char** (*get_children)(const char *path);

#endif
