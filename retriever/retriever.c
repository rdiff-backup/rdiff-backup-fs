#include "retriever.h"

int policy = POLICY_SIMPLE;
pthread_mutex_t **file_mutex = NULL;
int cache_limit = 0;

int retriever_init_common(int, int *);
int (*retrieve)(struct stats *stats);
int (*release)(struct stats *stats);

int retriever_init_simple(int repos, int *revisions){

	// printf("[Function: retriever_init_simple] Initializing simple retriever for %d repos;\n", repos);
	policy = POLICY_SIMPLE;
	retrieve = &retrieve_simple;
	release = &release_simple;
	return retriever_init_common(repos, revisions);

};

int retriever_init_limit(int repos, int *revisions){

	// printf("[Function: retriever_init_limit] Initializing limit retriever for %d repos;\n", repos);
	policy = POLICY_LIMIT;
	retrieve = &retrieve_limit;
	release = &release_limit;
	return retriever_init_common(repos, revisions);
	
};

int retriever_init_common(int repos, int *revisions){

	int i = 0, j = 0;

	// printf("[Function: retriever_init_common] Received %d repos;\n", repos);
	file_mutex = calloc(repos, sizeof(pthread_mutex_t *));
	for (i = 0; i < repos; i++){
		file_mutex[i] = calloc(revisions[i], sizeof(pthread_mutex_t));
		for (j = 0; j < revisions[i]; j++)
			pthread_mutex_init(&file_mutex[i][j], 0);
	};
	return 0;

};
