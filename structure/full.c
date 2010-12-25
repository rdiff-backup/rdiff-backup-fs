#include "full.h"

// prototypes:

int add_revs_dir(char *, char *);

int add_repo_dir(char *, int);

void read_revision_all(char *, char *, int, int);

int read_stats_all(struct stats *stats, char *prefix, int repo, int rev, FILE *file);

// variables:

struct node *structure_tree = NULL;

// public:

int full_build(char *repo){

#define full_build_finish(value) {					\
			gstrdel(path);							\
			if (revs != NULL){						\
				for (i = 0; i < rev_count[0]; i++)	\
					if (revs[i] != NULL)			\
						free(revs[i]);				\
				free(revs);							\
			};										\
			return value;							\
		}

    char **revs = NULL;
    char *extension = NULL;
    char *path = NULL;
    int i = 0;
	
	gtreenew(&structure_tree);
	if (gmstrcpy(&path, repo, "/rdiff-backup-data", 0) != 0)
		full_build_finish(-1);
	if ((rev_count = single(int)) == NULL)
		full_build_finish(-1);
    if ((rev_count[0] = unzip_revs(path)) == -1)
    	full_build_finish(-1);
    if ((revs = calloc(rev_count[0], sizeof(char *))) == NULL)
    	full_build_finish(-1);
    if (get_revisions(rev_count[0], revs) == -1)
    	full_build_finish(-1);
    gstrsort(revs, rev_count[0]);
    for (i = rev_count[0] - 1; i >= 0; i--){
    	extension = gpthext(revs[i]);
    	if (strcmp(extension, "snapshot") == 0)
    		snapshot_copy(revs[i]);
    	else // strcmp(extension, "diff") == 0)
    		snapshot_append(revs[i]);
		gstrdel(extension);
		read_revision_all(NULL, revs[i], -1, rev_count[0] - i - 1);
	};
	full_build_finish(0);

};

int full_build_multi(int count, char **repo){

#define full_build_multi_free_revs											\
			if (revs != NULL){												\
				for (k = 0; k < rev_count[i]; k++)							\
					if (revs[k] != NULL)									\
						free(revs[k]);										\
				free(revs);													\
				gmstrcpy(&snapshot, tmp_file, "/", CURRENT_SNAPSHOT, 0);	\
				unlink(snapshot);											\
				gstrdel(snapshot);											\
			}

#define full_build_multi_finish(value) {										\
			gstrdel(repo_dir);												\
			full_build_multi_free_revs;										\
			return value;													\
		}

    int i = 0;
    int j = 0;
    int k = 0;
    char **revs = NULL;
    char *repo_dir = NULL;
	char *extension = NULL;
	char *snapshot = NULL;

#ifdef DEBUG
    printf("[Function: init_multi] Received %d repos;\n", count);
#endif
	gtreenew(&structure_tree);
	if ((rev_count = calloc(repo_count, sizeof(int))) == NULL)
		full_build_multi_finish(-1);
    for (i = 0; i < repo_count; i++){
		if (gmstrcpy(&repo_dir, repos[i], "/rdiff-backup-data", NULL) == -1)
			continue;
		if ((rev_count[i] = unzip_revs(repo_dir)) == -1)
			continue;
		if ((revs = calloc(rev_count[i], sizeof(char *))) == NULL)
			continue;
		if (get_revisions(rev_count[i], revs) == -1){
			full_build_multi_free_revs;
			continue;
		};
		if (add_repo_dir(repo_names[i], i) == -1){
			full_build_multi_free_revs;
			continue;
		};
		for (j = rev_count[i] - 1; j >= 0; j--){
			extension = gpthext(revs[j]);
			if (strcmp(extension, "snapshot") == 0)
    			snapshot_copy(revs[j]);
    		else // strcmp(extension, "diff") == 0)
    			snapshot_append(revs[j]);
		    gstrdel(extension);
		    read_revision_all(repo_names[i], revs[j], i, rev_count[i] - j - 1);
		};
		full_build_multi_free_revs;
    };
    return 0;
};

int full_get_file(char *repo, char *revision, char *internal, stats_t **stats){
	
	char *path = NULL;
	int result = 0;

	set_path(path, repo, revision, internal);
	result = gtreeget(structure_tree, path, stats);
	gstrdel(path);
	return result;
	
};

char** full_get_children(char *repo, char *revision, char *internal){

	char *path = NULL;
	char **result = NULL;

	set_path(path, repo, revision, internal);
	result = gtreecld(structure_tree, path);
	gstrdel(path);
	return result;

};

void read_revision_all(char *repo, char *rev, int repo_index, int rev_index){

#define read_revision_error {						\
	    gstrdel(file);									\
	    gstrdel(rev_dir);								\
	    if (file != NULL)								\
	    	fclose(file);								\
	    return;										\
	}
	
#ifdef DEBUG
	printf("[Function: read_repo_revision] Received repo %s and revision %s;\n", repo, rev);
#endif
    char *rev_dir = NULL;
    char *file_path = NULL;
    FILE *file = NULL;
    struct stats *stats = calloc(1, sizeof(struct stats));

	if (repo == NULL){
		if (gmstrcpy(&rev_dir, "/", get_revs_dir(rev), 0) != 0)
			read_revision_error;
	}
	else{ // repo != NULL
    	if (gmstrcpy(&rev_dir, "/", repo, "/", get_revs_dir(rev), 0) != 0)
			read_revision_error;
	};
    if (gmstrcpy(&file_path, tmp_file, "/", CURRENT_SNAPSHOT, 0) != 0)
		read_revision_error;
    if ((file = fopen(file_path, "r")) == NULL)
		read_revision_error;
    add_revs_dir(rev, repo);

    while (read_stats_all(stats, rev_dir, repo_index, rev_index, file) == 0){
    	if (stats->type == -1)
    		gtreedel(structure_tree, stats->path);
    	else
			gtreeadd(structure_tree, stats);
	};

    fclose(file);
    gmstrcpy(&file_path, tmp_file, "/", rev, 0);
    unlink(file_path);
    gstrdel(file_path);
    gstrdel(rev_dir);
    free(stats);
    
};

int read_stats_all(struct stats *stats, char *prefix, int repo, int rev, FILE *file){

	if (read_stats(stats, file) != 0)
		return -1;
	if (gmstrcpy(&stats->path, prefix, "/", stats->internal, 0) == -1)
		return -1;
	gstrdel(stats->internal);
	if (gpthcldptr(&stats->name, stats->path) == -1)
		return -1;
	if (stats->type == S_IFDIR)
		stats->size = DIR_SIZE;
    stats->internal = stats->path + strlen(prefix) + strlen("/");
    stats->rev = rev;
#ifdef FULL_DEBUG
	printf("[Function: read_stats_all] Read data of file %s\n", stats->path);
#endif
    return 0;
   
};

int add_revs_dir(char *revision, char *repository){
    
    struct stats *stats = calloc(1, sizeof(struct stats));
    
    if (revision == NULL)
		return -1;
    if (repository == NULL){
		gmstrcpy(&stats->path, "/", get_revs_dir(revision), NULL);
#ifdef DEBUG_DEEP
		printf("[Function: add_revs_dir] Adding revision %s to the data structure;\n", stats->path);
#endif
		// stats->name = stats->path + strlen("/")'
		stats->name = stats->path + 1;
    }
    else{
		gmstrcpy(&stats->path, "/", repository, "/", get_revs_dir(revision), NULL);
		// stats->name = stats->path + strlen("/") + strlen(repository) + strlen("/");
		stats->name = stats->path + 1 + strlen(repository) + 1;
    };
	set_directory_stats(stats);
    return gtreeadd(structure_tree, stats);
    
};

int add_repo_dir(char *repository, int index){

	struct stats *stats = single(struct stats);

#ifdef DEBUG_DEEP
	printf("[Function: add_repo_dir] Adding repository %s;\n", repository);
#endif
	gmstrcpy(&stats->path, "/", repository, NULL);
	stats->name = stats->path + 1;
	stats->internal = NULL;
	stats->rev = -1;
	set_directory_stats(stats);
	gtreeadd(structure_tree, stats);

	return 0;

};
