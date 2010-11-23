#ifndef _LAYOUT_VERSIONS_H_
#define _LAYOUT_VERSIONS_H_

#include "../headers.h"
#include "core.h"
#include "../support/gstring.h"
#include "../support/gtree.h"
#include "../support/gstats.h"
#include "../support/grdiff.h"
#include "../structure/core.h"

int versions_init(char *);

int versions_init_multi(int, char **);

int versions_get_file(const char *path, struct stats **);

char** versions_get_children(const char *path);

#endif
