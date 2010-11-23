#ifndef _STRUCTURE_CORE_H_
#define _STRUCTURE_CORE_H_

#include "../headers.h"
#include "../support/gstats.h"

extern int (*struct_build)(char *repo);

extern int (*struct_build_multi)(int count, char **repos);

extern int (*struct_get_file)(char *, char *, char *, struct stats **);

extern char** (*struct_get_children)(char *repo, char *revision, char *internal);

#endif
