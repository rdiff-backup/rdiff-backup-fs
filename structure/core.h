#ifndef _STRUCTURE_CORE_H_
#define _STRUCTURE_CORE_H_

#include "../headers.h"
#include "../support/gstats.h"

extern int (*struct_build)(struct file_system_info *);

extern int (*struct_build_multi)(struct file_system_info *);

extern int (*struct_get_file)(struct file_system_info *, char *, char *, char *, struct stats **);

extern char** (*struct_get_children)(struct file_system_info *, char *repo, char *revision, char *internal);

#endif
