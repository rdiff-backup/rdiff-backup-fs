#include "core.h"

int (*struct_build)(struct file_system_info *) = NULL;

int (*struct_build_multi)(struct file_system_info *) = NULL;

int (*struct_get_file)(struct file_system_info *, char *repo, char *revision, char *internal, struct stats **stats) = NULL;

char** (*struct_get_children)(struct file_system_info *, char *repo, char *revision, char *internal) = NULL;
