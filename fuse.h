#include "headers.h"

#include <fuse.h>

#include "support/gstring.h"
#include "support/gstats.h"
#include "externs.h"
#include "errors.h"
#include "retriever/retriever.h"
#include "layout/core.h"

int revs_getattr(const char *path, struct stat *stbuf);

int revs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);

int revs_readlink(const char *, char *, size_t);

int revs_open(const char *path, struct fuse_file_info *fi);

int revs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

int revs_release(const char *path, struct fuse_file_info *fi);

void revs_destroy(void *);
