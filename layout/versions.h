#ifndef _LAYOUT_VERSIONS_H_
#define _LAYOUT_VERSIONS_H_

#include "../headers.h"
#include "../externs.h"
#include "core.h"
#include "../support/gstring.h"
#include "../support/gtree.h"
#include "../support/gstats.h"
#include "../support/grdiff.h"
#include "../structure/core.h"

int versions_init(struct file_system_info *fsinfo);

int versions_init_multi(struct file_system_info *fsinfo);

int versions_get_file(struct file_system_info *fsinfo, const char *path, struct stats **);

char** versions_get_children(struct file_system_info *fsinfo, const char *path);

#endif
