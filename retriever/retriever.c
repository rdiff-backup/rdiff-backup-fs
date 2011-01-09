#include "retriever.h"
#include "support/gutils.h"

int policy = POLICY_SIMPLE;
pthread_mutex_t **file_mutex = NULL;
int cache_limit = 0;

int retriever_init_common(struct file_system_info *);
int (*retrieve)(struct file_system_info *fsinfo, stats_t *stats);
int (*release)(struct file_system_info *fsinfo, stats_t *stats);

int retriever_init_simple(struct file_system_info *fsinfo){

	debug(3, "[Function: retriever_init_simple] Initializing simple retriever for %d repos;\n", fsinfo->repo_count);
	policy = POLICY_SIMPLE;
	retrieve = &retrieve_simple;
	release = &release_simple;
	return retriever_init_common(fsinfo);

};

int retriever_init_limit(struct file_system_info *fsinfo){

	debug(3, "[Function: retriever_init_limit] Initializing limit retriever for %d repos;\n", fsinfo->repo_count);
	policy = POLICY_LIMIT;
	retrieve = &retrieve_limit;
	release = &release_limit;
	return retriever_init_common(fsinfo);
	
};

int retriever_init_common(struct file_system_info *fsinfo){

	int i = 0, j = 0;

	debug(3, "[Function: retriever_init_common] Received %d repos;\n", fsinfo->repo_count);
	file_mutex = calloc(fsinfo->repo_count, sizeof(pthread_mutex_t *));
	for (i = 0; i < fsinfo->repo_count; i++){
		file_mutex[i] = calloc(fsinfo->rev_count[i], sizeof(pthread_mutex_t));
		for (j = 0; j < fsinfo->rev_count[i]; j++)
			pthread_mutex_init(&file_mutex[i][j], 0);
	};
	return 0;

};
