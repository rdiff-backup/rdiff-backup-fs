#ifndef _EXTERNS_H_
#define _EXTERNS_H_

#define FUSE_USE_VERSION 26

#include <fuse.h>

extern char *mount;
// directory where data directory should be created
extern char *tmp_dir;
// directory where file system's data will be kept
extern char *data_dir;

extern struct fuse_operations operations;
extern struct file_system_info *file_system_info;

extern int structure;
extern int layout;
extern int debug;

extern int policy;

#endif
