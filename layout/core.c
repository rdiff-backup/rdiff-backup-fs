#include "core.h"

int (*init)(char *repo) = NULL;

int (*init_multi)(int count, char **repos) = NULL;

int (*get_file)(const char *path, struct stats **stats) = NULL;

char** (*get_children)(const char *path) = NULL;
