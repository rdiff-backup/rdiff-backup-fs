#include <fuse.h>

extern int repo_count;
extern int *rev_count;
extern char **repos;
extern char **repo_names;

extern char *mount;
// directory where data directory should be created
extern char *tmp_dir;
// directory where file system's data will be kept
extern char *data_dir;

extern struct fuse_operations operations;

extern int structure;
extern int layout;
extern int debug;

extern int policy;
