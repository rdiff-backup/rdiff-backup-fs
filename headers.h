#ifndef _HEADERS_H_
#define _HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <zlib.h>
#include <stdarg.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define FUSE_USE_VERSION 26

#include <fuse.h>

#define pass(val) printf("Passed point %d\n", val);
#define single(type) calloc(1, sizeof(type))

#define DEBUG

#define PROGRAM_NAME "rdiff-backup-fs"
#define TMP_DIR_NAME PROGRAM_NAME "-XXXXXX"

#define MIRROR_PREFIX "mirror_metadata."
// strlen("mirror_metadata.XXXX-XX-XXTXX:XX:XX");
#define GMT_TYPE_POSITION 35

#define DIR_SIZE 4096

#define STRUCTURE_FULL 1
#define STRUCTURE_NECESSARY 2

#define LAYOUT_ALL 1
#define LAYOUT_LAST 2

/* type of policy used in caching files */
#define POLICY_SIMPLE 1
#define POLICY_LIMIT 2

#define ARCHFS_DIR_FORMAT "%4d-%02d-%02dT%02d:%02d:%02d"
#define ARCHFS_DIR_FORMAT_LENGTH "XXXX-XX-XXTXX:XX:XX"

#define lock(mutex) pthread_mutex_lock(&mutex)
#define unlock(mutex) pthread_mutex_unlock(&mutex)

struct file_system_info {
	int repo_count;
	int *rev_count;
    
    char **repos;
    char **repo_names;
    
    // TODO: this must be fixed; it doesn't hold actual information about revs,
    // but only the most recent revs for multiple repos
    char **revs;
};

#endif
