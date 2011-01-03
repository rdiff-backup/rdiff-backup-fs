#include "support.h"
#include "../headers.h"
#include "../externs.h"
#include "../support/gstats.h"
#include "../support/gtree.h"
#include "../support/grdiff.h"

int full_build(struct file_system_info *, char *);

int full_build_multi(struct file_system_info *, char **);

int full_get_file(struct file_system_info *, char *, char *, char *, struct stats **);

char** full_get_children(struct file_system_info *, char *, char *, char *);
