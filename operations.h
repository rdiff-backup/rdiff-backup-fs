#ifndef _OPERATIONS_H_
#define _OPERATIONS_H_

#define FUSE_USE_VERSION 31

#include <fuse3/fuse.h>

int revs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fileinfo);

int revs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi,  enum fuse_readdir_flags flags);

int revs_readlink(const char *, char *, size_t);

int revs_open(const char *path, struct fuse_file_info *fi);

int revs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

int revs_release(const char *path, struct fuse_file_info *fi);

void revs_destroy(void *);

#endif
