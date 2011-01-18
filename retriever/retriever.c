#include "retriever.h"
#include "support.h"
#include "support/gutils.h"

int policy = POLICY_SIMPLE;
pthread_mutex_t **file_mutex = NULL;
int cache_limit = 0;

int (*retrieve)(struct file_system_info *fsinfo, stats_t *stats);
int (*release)(struct file_system_info *fsinfo, stats_t *stats);

int retriever_init_simple(struct file_system_info *fsinfo){

	debug(3, "Initializing simple retriever for %d repos;\n", fsinfo->repo_count);
	policy = POLICY_SIMPLE;
	retrieve = &retrieve_simple;
	release = &release_simple;
	return retriever_init_common(fsinfo);

};

int retriever_init_limit(struct file_system_info *fsinfo){

	debug(3, "Initializing limit retriever for %d repos;\n", fsinfo->repo_count);
	policy = POLICY_LIMIT;
	retrieve = &retrieve_limit;
	release = &release_limit;
	return retriever_init_common(fsinfo);
	
};
