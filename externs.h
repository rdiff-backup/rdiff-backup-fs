#include <fuse.h>
#include "support/gstring.h"

extern int repo_count;
extern int *rev_count;
extern char **repos;
extern char **repo_names;

extern char *mount;
extern char *tmp_dir;
extern char *tmp_file;

extern struct fuse_operations operations;

extern int structure;
extern int layout;
extern int debug;

extern int policy;
