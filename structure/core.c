#include "core.h"

int (*struct_build)(char *repo) = NULL;

int (*struct_build_multi)(int count, char **repos) = NULL;

int (*struct_get_file)(char *repo, char *revision, char *internal, struct stats **stats) = NULL;

char** (*struct_get_children)(char *repo, char *revision, char *internal) = NULL;
