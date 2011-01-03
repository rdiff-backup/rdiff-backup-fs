#ifndef _LAYOUT_CORE_H_
#define _LAYOUT_CORE_H_

#include "../headers.h"
#include "../support/gstats.h"

extern int (*init)(struct file_system_info *, char *repo);

extern int (*init_multi)(struct file_system_info *, char **repos);

extern int (*get_file)(struct file_system_info *, const char *path, struct stats **stats);

extern char** (*get_children)(struct file_system_info *, const char *path);

#endif
