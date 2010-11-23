#include "support.h"

int retrieve_common(struct stats *stats, int repo){

#define retrieve_common_finish(value){								\
			unlock(file_mutex[repo][stats->rev]);			\
			gstrdel(file);									\
			gstrdel(revision);								\
            free(temp);                                     \
			return value;									\
		}

	char *file = NULL;
	char *revision = calloc(20, sizeof(char));
	struct stat *temp = single(struct stat);

#ifdef DEBUG_DEEP
	printf("[Function: retrieve_common] Received file %s from repo %d;\n", stats->path, repo);
#endif
	lock(file_mutex[repo][stats->rev]);
	if (stats->shared > 0){
		stats->shared++;
		retrieve_common_finish(0);
	};
	if (create_tmp_file(stats) == -1)
		retrieve_common_finish(-1);
	if (gmstrcpy(&file, repos[repo], "/", stats->internal, 0) == -1)
		retrieve_common_finish(-1);
	sprintf(revision, "%dB", stats->rev);
	if (retrieve_rdiff(revision, file, stats->tmp_path) != 0)
		retrieve_common_finish(-1);
	if (stat(stats->tmp_path, temp) != 0)
		retrieve_common_finish(-1);
	stats->shared = 1;
	retrieve_common_finish(0);

};

int repo_number(struct stats *stats){

#define repo_number_finish(value) {				\
			gstrdel(repo);						\
			return value;						\
		}

	int i = 0;
	char *repo = NULL;

#ifdef DEBUG_DEEP
	printf("[Function: repo_number] Received file %s;\n", stats->path);
#endif
	if (repo_count == 1)
		repo_number_finish(0);
	if ((repo = gpthprt(stats->path, 0)) == NULL)
		repo_number_finish(-1);
	for (i = 0; (i < repo_count) && (strcmp(repo, repo_names[i]) != 0); i++);
	if (i == repo_count)
		repo_number_finish(-1);
	repo_number_finish(i);
	
};

int retrieve_rdiff(char *revision, char *file, char *tmp_path){

	int pid = 0;

#ifdef DEBUG_DEEP
	printf("[Function: retrieve_rdiff] Received file %s from revision %s retrieved to path %s\n", file, revision, tmp_path);
#endif
    if ((pid = fork()) == -1)
		return -1;
    if (pid == 0){
    	printf("%s %s %s\n", revision, file, tmp_path);
		if (execlp("rdiff-backup", "rdiff-backup", "--force", "-r", revision, file, tmp_path, NULL) == -1)
		    fail(ERR_RDIFF);
	};
    wait(0);
    return 0;
	
};

int create_tmp_file(struct stats *stats){

#define create_tmp_file_error {				\
	    gstrdel(tmp_template);				\
	    if (desc != -1)						\
			close(desc);					\
	    return -1;							\
	}

    char *tmp_template = NULL;
    int desc = -1;

#ifdef DEBUG_DEEP
	printf("[Function: create_tmp_file] Received file %s;\n", stats->path);
#endif
    if (gmstrcpy(&tmp_template, tmp_file, "/", stats->name, "XXXXXX", 0) != 0)
		create_tmp_file_error;
    desc = mkstemp(tmp_template);
    if (desc == -1)
		create_tmp_file_error;
    close(desc);
    if (gstrcpy(&stats->tmp_path, tmp_template) != 0)
		create_tmp_file_error;
    gstrdel(tmp_template);
    return 0;

};
