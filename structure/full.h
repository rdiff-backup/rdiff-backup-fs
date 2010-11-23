#include "support.h"
#include "../headers.h"
#include "../externs.h"
#include "../support/gstats.h"
#include "../support/gtree.h"
#include "../support/grdiff.h"

int full_build(char *);

int full_build_multi(int, char **);

int full_get_file(char *, char *, char *, struct stats **);

char** full_get_children(char *, char *, char *);
