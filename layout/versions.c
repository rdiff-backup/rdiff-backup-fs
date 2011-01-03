#include "versions.h"

// variables

struct node *version_tree = NULL;

// prototypes

int read_layout_versions(char *, char *);

void read_revision_versions(char *rev, int rev_index, char *);

#define CURRENT_SNAPSHOT "mirror_metadata.current.snapshot"

// public:

int versions_init(struct file_system_info *fsinfo){

    int i = 0;

	// printf("[Function: init_versions] Received repo path %s;\n", path);
	gtreenew(&version_tree);
	fsinfo->rev_count = single(int);
    if ((fsinfo->rev_count[0] = gather_revisions(fsinfo, fsinfo->repos[0], data_dir)) <= 0)
        return -1;
    read_layout_versions(fsinfo->revs[fsinfo->rev_count[0] - 1], NULL);
    for (i = fsinfo->rev_count[0] - 1; i >= 0; i--){
        add_snapshot(fsinfo->revs[i], CURRENT_SNAPSHOT, data_dir);
		read_revision_versions(fsinfo->revs[i], fsinfo->rev_count[0] - i - 1, NULL);
	};
    return 0;

};

int versions_init_multi(struct file_system_info *fsinfo){

#define versions_init_multi_free_revs										\
			if (fsinfo->revs != NULL){										\
				gmstrcpy(&snapshot, data_dir, "/", CURRENT_SNAPSHOT, 0);	\
				unlink(snapshot);											\
				gstrdel(snapshot);											\
			}

#define versions_init_multi_finish(value) {									\
			init_versions_multi_free_revs;									\
			return value;													\
		}

	int i = 0, j = 0;
	char *snapshot = NULL;

	gtreenew(&version_tree);
	fsinfo->rev_count = calloc(fsinfo->repo_count, sizeof(int));
	for (i = 0; i < fsinfo->repo_count; i++){
        if ((fsinfo->rev_count[i] = gather_revisions(fsinfo, fsinfo->repos[i], data_dir)) == -1)
            continue;
		if (add_repo_dir(fsinfo->repo_names[i], version_tree) == -1){
			versions_init_multi_free_revs;
			continue;
		};
		if (read_layout_versions(fsinfo->revs[fsinfo->rev_count[i] - 1], fsinfo->repo_names[i]) == -1){
			versions_init_multi_free_revs;
			continue;
		};
		for (j = fsinfo->rev_count[i] - 1; j >= 0; j--){
            add_snapshot(fsinfo->revs[j], CURRENT_SNAPSHOT, data_dir);
		    read_revision_versions(fsinfo->revs[j], fsinfo->rev_count[i] - j - 1, fsinfo->repo_names[i]);
		};
		versions_init_multi_free_revs;
	};
	return 0;

};

int versions_get_file(struct file_system_info *fsinfo, const char *path, struct stats **stats){
    
    (void) fsinfo;
    
	return gtreeget(version_tree, path, stats);
};

char** versions_get_children(struct file_system_info *fsinfo, const char *path){
    
    (void) fsinfo;
    
	return gtreecld(version_tree, path);
};

// private:

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
		gtreeadd(version_tree, &stats, stats.path);
	};
	read_layout_versions_finish(0);

};

void read_revision_versions(char *revision, int rev_index, char *prefix){

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
			gtreeadd(version_tree, &stats, stats.path);
		};
	};
	read_revision_versions_finish;
			
};

