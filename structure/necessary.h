#include "support.h"
#include "../headers.h"
#include "../externs.h"
#include "../support/gstats.h"
#include "../support/gtree.h"
#include "../support/grdiff.h"

#ifndef _NECESSARY_H_
#define _NECESSARY_H_

#define DEFAULT_NECESSARY_LIMIT 10

extern int necessary_limit;

int necessary_build(char *);

int necessary_build_multi(int, char **);

int necessary_get_file(char *, char *, char *, struct stats **);
				  
char** necessary_get_children(char *, char *, char *);

#endif
