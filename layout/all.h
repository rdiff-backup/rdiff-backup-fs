#ifndef _LAYOUT_ALL_H_
#define _LAYOUT_ALL_H_

#include "../headers.h"
#include "../externs.h"
#include "../support/gstring.h"
#include "../support/gpath.h"
#include "../structure/core.h"


int all_init(struct file_system_info *fsinfo, char *);

int all_init_multi(struct file_system_info *fsinfo, char **);

int all_get_file(struct file_system_info *fsinfo, const char *, struct stats **);

char** all_get_children(struct file_system_info *fsinfo, const char *);

#endif
