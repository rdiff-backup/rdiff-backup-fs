#include "full.h"

// prototypes:

int add_revs_dir(char *, char *);

void read_revision_all(char *, char *, int);

int read_stats_all(struct stats *stats, char *prefix, int rev, FILE *file);

#define CURRENT_SNAPSHOT "mirror_metadata.current.snapshot"

// variables:

tree_t structure_tree = NULL;

// public:

int full_build(struct file_system_info *fsinfo, char *repo){

#define full_build_finish(value) {					\
            gstrlistdel(revs, fsinfo->rev_count[0]);            \
			return value;							\
		}

    char **revs = NULL;
    int i = 0;
	
    gtreenew(&structure_tree);
	if ((fsinfo->rev_count = single(int)) == NULL)
		full_build_finish(-1);
    if ((fsinfo->rev_count[0] = gather_revisions(repo, data_dir, &revs)) == -1)
        full_build_finish(-1);
    for (i = fsinfo->rev_count[0] - 1; i >= 0; i--){
        add_snapshot(revs[i], CURRENT_SNAPSHOT, data_dir);
		read_revision_all(NULL, revs[i], fsinfo->rev_count[0] - i - 1);
	};
	full_build_finish(0);

};

int full_build_multi(struct file_system_info *fsinfo, char **repos){

#define full_build_multi_free_revs											\
            if (revs != NULL){												\
                gstrlistdel(revs, fsinfo->rev_count[i])                             \
				gmstrcpy(&snapshot, data_dir, "/", CURRENT_SNAPSHOT, 0);	\
				unlink(snapshot);											\
				gstrdel(snapshot);											\
			}

#define full_build_multi_finish(value) {										\
			gstrdel(repo_dir);												\
			full_build_multi_free_revs;										\
			return value;													\
		}

    int i = 0, j = 0;
    char **revs = NULL;
    char *repo_dir = NULL;
	char *snapshot = NULL;

    //printf("[Function: init_multi] Received %d repos;\n", count);
	gtreenew(&structure_tree);
	if ((fsinfo->rev_count = calloc(fsinfo->repo_count, sizeof(int))) == NULL)
		full_build_multi_finish(-1);
    for (i = 0; i < fsinfo->repo_count; i++){
        if ((fsinfo->rev_count[i] = gather_revisions(repos[i], data_dir, &revs)) == -1)
            continue;
		if (add_repo_dir(repo_names[i], structure_tree) == -1){
			full_build_multi_free_revs;
			continue;
		};
		for (j = fsinfo->rev_count[i] - 1; j >= 0; j--){
            add_snapshot(revs[j], CURRENT_SNAPSHOT, data_dir);
		    read_revision_all(repo_names[i], revs[j], fsinfo->rev_count[i] - j - 1);
		};
		full_build_multi_free_revs;
    };
    return 0;
};

int full_get_file(struct file_system_info *fsinfo, char *repo, char *revision, char *internal, stats_t **stats){
	
	char *path = NULL;
	int result = 0;

	set_path(path, repo, revision, internal);
	result = gtreeget(structure_tree, path, stats);
	gstrdel(path);
	return result;
	
};

char** full_get_children(struct file_system_info *fsinfo, char *repo, char *revision, char *internal){

	char *path = NULL;
	char **result = NULL;

	set_path(path, repo, revision, internal);
	result = gtreecld(structure_tree, path);
	gstrdel(path);
	return result;

};

// private:

void read_revision_all(char *repo, char *rev, int rev_index){

#define read_revision_error {						\
	    gstrdel(file);									\
	    gstrdel(rev_dir);								\
	    if (file != NULL)								\
	    	fclose(file);								\
	    return;										\
	}
	
	// printf("[Function: read_repo_revision] Received repo %s and revision %s;\n", repo, rev);
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
    if (gmstrcpy(&file_path, data_dir, "/", CURRENT_SNAPSHOT, 0) != 0)
		read_revision_error;
    if ((file = fopen(file_path, "r")) == NULL)
		read_revision_error;
    add_revs_dir(rev, repo);

    while (read_stats_all(stats, rev_dir, rev_index, file) == 0)
        update_tree(structure_tree, stats);

    fclose(file);
    gmstrcpy(&file_path, data_dir, "/", rev, 0);
    unlink(file_path);
    gstrdel(file_path);
    gstrdel(rev_dir);
    free(stats);
    
};

int read_stats_all(struct stats *stats, char *prefix, int rev, FILE *file){

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
		// printf("[Function: add_revs_dir] Adding revision %s to the data structure;\n", stats->path);
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
