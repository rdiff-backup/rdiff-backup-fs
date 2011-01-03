#include "all.h"

// prototypes:

int get_internals(struct file_system_info *, const char *path, char **repo, char **revision, char **internal);

// public:

int all_init(struct file_system_info *fsinfo){
	return struct_build(fsinfo);
};

int all_init_multi(struct file_system_info *fsinfo){
	return struct_build_multi(fsinfo);
};

int all_get_file(struct file_system_info *fsinfo, const char *path, struct stats **stats){

	char *revision = NULL;
	char *repo = NULL;
	char *internal = NULL;

    // printf("[all_get_file] getting file stats for %s\n", path);
	if (get_internals(fsinfo, path, &repo, &revision, &internal) != 0)
		return -1;
	int result = struct_get_file(fsinfo, repo, revision, internal, stats);
    free(revision);
    free(repo);
    free(internal);
    // printf("[all_get_file] stats retrieved\n");
    return result;
	
};

char** all_get_children(struct file_system_info *fsinfo, const char *path){
	char *revision = NULL;
	char *repo = NULL;
	char *internal = NULL;
	
	if (get_internals(fsinfo, path, &repo, &revision, &internal) != 0)
		return NULL;
	char** result = struct_get_children(fsinfo, repo, revision, internal);
    free(revision);
    free(repo);
    free(internal);
    return result;
};

// private:

int get_internals(struct file_system_info *fsinfo, const char *path, char **repo, char **revision, char **internal){

	char *temp = NULL;

	(*repo) = NULL;
	(*revision) = NULL;
	(*internal) = NULL;

	if (strcmp(path, "/") == 0)
		gstrcpy(internal, "/");
	else{
		if (fsinfo->repo_count == 1){
			if (((*revision) = gpthprt(path, 0)) == NULL)
				return -1;
			(*internal) = gpthcut(path);
		}
		else{
			if (((*repo) = gpthprt(path, 0)) == NULL)
				return -1;
			if (((*revision) = gpthprt(path, 1)) != NULL){
				if ((temp = gpthcut(path)) == NULL){
					gstrdel(repo);
					gstrdel(revision);
					return -1;
				};
				(*internal) = gpthcut(temp);
			};
		};
	};
	return 0;

};
