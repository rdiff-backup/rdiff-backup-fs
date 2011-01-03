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

int necessary_build(struct file_system_info *);

int necessary_build_multi(struct file_system_info *);

int necessary_get_file(struct file_system_info *, char *, char *, char *, struct stats **);
				  
char** necessary_get_children(struct file_system_info *, char *, char *, char *);

#endif
