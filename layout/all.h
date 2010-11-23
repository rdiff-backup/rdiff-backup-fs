#ifndef _LAYOUT_ALL_H_
#define _LAYOUT_ALL_H_

#include "../headers.h"
#include "../externs.h"
#include "../support/gstring.h"
#include "../support/gpath.h"
#include "../structure/core.h"

#ifdef DEBUG_ON
    #define ALL_DEBUG
    // #define ALL_DEBUG_DEEP
#endif

int all_init(char *);

int all_init_multi(int, char **);

int all_get_file(const char *, struct stats **);

char** all_get_children(const char *);

#endif
