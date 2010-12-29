#include "versions.h"

// variables

struct node *version_tree = NULL;

// prototypes

int read_layout_versions(char *, char *);

void read_revision_versions(char *rev, int rev_index, char *, int);

int versions_add_repo_dir(char *, int);

// public:

int versions_init(char *repo){

#define versions_init_finish(value) {						\
			gstrdel(path);									\
			if (revs != NULL){								\
				for (i = 0; i < rev_count[0]; i++)			\
					if (revs[i] != NULL)					\
						free(revs[i]);						\
				free(revs);									\
			};												\
			return value;									\
		}

    char **revs = NULL;
    char *path = NULL;
    char *extension = NULL;
    int i = 0;

	// printf("[Function: init_versions] Received repo path %s;\n", path);
	gtreenew(&version_tree);
	rev_count = single(int);

	if (gmstrcpy(&path, repo, "/rdiff-backup-data", 0) != 0)
		versions_init_finish(-1);
	rev_count[0] = unzip_revs(path, data_dir);

    revs = calloc(rev_count[0], sizeof(char *));
    get_revisions(rev_count[0], revs);
    read_layout_versions(revs[rev_count[0] - 1], NULL);
    for (i = rev_count[0] - 1; i >= 0; i--){
    	extension = gpthext(revs[i]);
    	if (strcmp(extension, "snapshot") == 0)
    		snapshot_copy(revs[i]);
    	else // strcmp(extension, "diff") == 0)
    		snapshot_append(revs[i]);
		gstrdel(extension);
		read_revision_versions(revs[i], rev_count[0] - i - 1, NULL, -1);
	};
	versions_init_finish(0);

};

int versions_init_multi(int count, char **repos){

#define versions_init_multi_free_revs										\
			if (revs != NULL){												\
				for (k = 0; k < rev_count[i]; k++)							\
					if (revs[k] != NULL)									\
						free(revs[k]);										\
				free(revs);													\
				gmstrcpy(&snapshot, data_dir, "/", CURRENT_SNAPSHOT, 0);	\
				unlink(snapshot);											\
				gstrdel(snapshot);											\
			}

#define versions_init_multi_finish(value) {									\
			init_versions_multi_free_revs;									\
			return value;													\
		}

	int i = 0, j = 0, k = 0;
	char *path = NULL;
	char **revs = NULL;
	char *extension = NULL;
	char *snapshot = NULL;

	gtreenew(&version_tree);
	rev_count = calloc(repo_count, sizeof(int));
	for (i = 0; i < repo_count; i++){
		if (gmstrcpy(&path, repos[i], "/rdiff-backup-data", 0) == -1)
			continue;
		if ((rev_count[i] = unzip_revs(path, data_dir)) == -1)
			continue;
		if ((revs = calloc(rev_count[i], sizeof(char *))) == NULL)
			continue;
		if (get_revisions(rev_count[i], revs) == -1){
			versions_init_multi_free_revs;
			continue;
		};
		if (versions_add_repo_dir(repo_names[i], i) == -1){
			versions_init_multi_free_revs;
			continue;
		};
		if (read_layout_versions(revs[rev_count[i] - 1], repo_names[i]) == -1){
			versions_init_multi_free_revs;
			continue;
		};
		for (j = rev_count[i] - 1; j >= 0; j--){
	    	extension = gpthext(revs[j]);
	    	if (strcmp(extension, "snapshot") == 0)
	    		snapshot_copy(revs[j]);
	    	else // strcmp(extension, "diff") == 0)
	    		snapshot_append(revs[j]);
			gstrdel(extension);
		    read_revision_versions(revs[j], rev_count[i] - j - 1, repo_names[i], i);
		};
		versions_init_multi_free_revs;
	};
	return 0;

};

int versions_get_file(const char *path, struct stats **stats){
	return gtreeget(version_tree, path, stats);
};

char** versions_get_children(const char *path){
	return gtreecld(version_tree, path);
};

// private:

int versions_add_repo_dir(char *repository, int index){

	struct stats *stats = single(struct stats);

#ifdef DEBUG_DEEP
	printf("[Function: add_repo_dir] Adding repository %s;\n", repository);
#endif
	gmstrcpy(&stats->path, "/", repository, NULL);
	stats->name = stats->path + 1;
	stats->internal = NULL;
	stats->rev = -1;
	set_directory_stats(stats);
	gtreeadd(version_tree, stats);

	return 0;

};

int read_layout_versions(char *revision, char *prefix){

#define read_layout_versions_finish(value) {				\
			gstrdel(path);									\
			if (file != NULL)								\
				fclose(file);								\
			return value;									\
		}

	char *path = NULL;
	FILE *file = NULL;
	struct stats stats;
		
	if (gmstrcpy(&path, data_dir, "/", revision, 0) != 0)
		read_layout_versions_finish(-1);
	if ((file = fopen(path, "r")) == NULL)
		read_layout_versions_finish(-1);
	// reading only from single snapshot;
	while ((read_stats(&stats, file) == 0) && ((stats.type == S_IFREG) || (stats.type == S_IFDIR))){
		if (prefix == NULL)
			gmstrcpy(&stats.path, "/", stats.internal, 0);
		else
			gmstrcpy(&stats.path, prefix, "/", stats.internal, 0);
		gpthcldptr(&stats.name, stats.path);
		stats.type = S_IFDIR;
		stats.ctime = time(0);
		stats.atime = time(0);
		stats.size = DIR_SIZE;
		gtreeadd(version_tree, &stats);
	};
	read_layout_versions_finish(0);

};

void read_revision_versions(char *revision, int rev_index, char *prefix, int repo){

#define read_revision_versions_finish {						\
			gmstrcpy(&path, data_dir, "/", revision, 0);	\
			unlink(path);									\
			gstrdel(path);									\
			gstrdel(sufix);									\
			gstrdel(file_path);								\
			if (file != NULL)								\
				fclose(file);								\
			return;										\
		}

	char *path = NULL;
	char *sufix = get_revs_dir(revision);
	char *file_path = NULL;
	struct stats stats, *temp;
	FILE *file = NULL;

	if (gmstrcpy(&path, data_dir, "/", CURRENT_SNAPSHOT, 0) != 0)
		read_revision_versions_finish;
	if ((file = fopen(path, "r")) == NULL)
		read_revision_versions_finish;
	while (read_stats(&stats, file) == 0){
		if (prefix == NULL)
			gstrcpy(&file_path, stats.internal);
		else // prefix != NULL
			gmstrcpy(&file_path, "/", prefix, "/", stats.internal, 0);
		if (prefix == NULL)
			gmstrcpy(&stats.path, stats.internal, "/", sufix, 0);
		else // prefix != NULL
			gmstrcpy(&stats.path, prefix, "/", stats.internal, "/", sufix, 0);
		if (stats.type == -1)
			gtreedel(version_tree, stats.path);
		else if ((gtreeget(version_tree, file_path, &temp) == 0) && (stats.type == S_IFREG)){
			gpthcldptr(&stats.name, stats.path);
			stats.rev = rev_index;
			gtreeadd(version_tree, &stats);
		};
	};
	read_revision_versions_finish;
			
};

