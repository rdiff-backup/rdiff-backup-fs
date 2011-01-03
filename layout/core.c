#include "core.h"

int (*init)(struct file_system_info *fsinfo) = NULL;

int (*init_multi)(struct file_system_info *fsinfo) = NULL;

int (*get_file)(struct file_system_info *fsinfo, const char *path, struct stats **stats) = NULL;

char** (*get_children)(struct file_system_info *fsinfo, const char *path) = NULL;
