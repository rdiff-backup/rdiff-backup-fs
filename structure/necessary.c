#include "necessary.h"
#include "../headers.h"

int necessary_limit = DEFAULT_NECESSARY_LIMIT;

struct revision {
	char *name;
    char *file;
	tree_t tree;
};

typedef struct revision revision_t;

struct repository {
    char *name;
    struct revision *revisions;
};

typedef struct repository repository_t;

repository_t *repositories = NULL;
static struct stats root;

/* private functions' prototypes */

static struct node * get_revision_tree(struct file_system_info *, char *, char *);

// static void free_revision_tree(char *repo, char *revision);

static int build_revision_tree(struct file_system_info *fsinfo, char *, revision_t *, int, int);

static int find_snapshot(revision_t *, int, int);

static char * build_snapshot(revision_t *, int, int);

static int free_cache();

int read_revision_necessary(char *, char *, tree_t, int);

/*
 * retrieves index of a repo from name
 */
 static int repo_index(struct file_system_info *, char *);

/*
 * checks whether repo with given name exists
 */
static int repo_exists(struct file_system_info *, char *repo_name);

/*
 * checks whether revision in given repo exists
 */
static int revision_exists(struct file_system_info *, char *repo_name, char *revision_name);

/* public functions */

int necessary_build(struct file_system_info *fsinfo){

    int i = 0;

    if ((fsinfo->rev_count[0] = gather_revisions(fsinfo, fsinfo->repos[0], data_dir)) == -1)
        return -1;
    if ((repositories = single(repository_t)) == NULL)
        return -1;
    repositories[0].revisions = calloc(fsinfo->rev_count[0], sizeof(revision_t));
    for (i = 0; i < fsinfo->rev_count[0]; i++) {
        repositories[0].revisions[i].name = get_revs_dir(fsinfo->revs[i]);
        gstrcpy(&repositories[0].revisions[i].file, fsinfo->revs[i]);
    }
    set_directory_stats(&root);
	return 0;
}

int necessary_build_multi(struct file_system_info *fsinfo){
    
    int i = 0, j = 0;
    
    if ((repositories = calloc(fsinfo->repo_count, sizeof(revision_t))) == NULL)
        return -1;
    for (i = 0; i < fsinfo->repo_count; i++){
        gstrcpy(&repositories[i].name, fsinfo->repo_names[i]);
        if ((fsinfo->rev_count[i] = gather_revisions(fsinfo, fsinfo->repos[i], data_dir)) == -1)
            return -1;
        repositories[i].revisions = calloc(fsinfo->rev_count[i], sizeof(revision_t));
        for (j = 0; j < fsinfo->rev_count[i]; j++){
            repositories[i].revisions[j].name = get_revs_dir(fsinfo->revs[j]);
            gstrcpy(&repositories[i].revisions[j].file, fsinfo->revs[j]);
        }
    }
    set_directory_stats(&root);
    return -1;
}

int necessary_get_file(struct file_system_info *fsinfo, char *repo, char *revision, char *internal, 
					   stats_t **stats){

    printf("[necessary_get_file: checking file %s/%s/%s\n", repo, revision, internal);
    if (revision == NULL && (repo == NULL || repo_exists(fsinfo, repo))){
        *stats = &root;
        return 0;
    }
    else if (revision != NULL && repo != NULL && internal == NULL){
        if (!revision_exists(fsinfo, repo, revision))
            return -1;
        else {
            *stats = &root;
            return 0;
        }
    }
    else{
        struct node *tree = get_revision_tree(fsinfo, repo, revision);
        if (!tree)
            return -1;
        return gtreeget(tree, internal, stats);
    }
}

char** necessary_get_children(struct file_system_info *fsinfo, char *repo, char *revision, char *internal){
    
    int i = 0, index = 0;
    char **result = 0;

#ifdef DEBUG
    printf("[necessary_get_children: getting children of %s/%s/%s\n", repo, revision, internal);
#endif
    if (revision != NULL && repo == NULL && fsinfo->repo_count > 1)
        return NULL; // should not happen
    if (revision == NULL && repo == NULL && fsinfo->repo_count > 1){
        result = calloc(fsinfo->repo_count + 1, sizeof(char *));
        for (i = 0; i < fsinfo->repo_count; i++)
            gstrcpy(&result[i], repositories[i].name);
        return result;
    }
    else if (revision == NULL && (fsinfo->repo_count == 1 || repo != NULL)){
        index = 0;
        if (repo && ((index = repo_index(fsinfo, repo)) == -1))
            return NULL;
        result = calloc(fsinfo->rev_count[index] + 1, sizeof(char *));
        for (i = 0; i < fsinfo->rev_count[index]; i++)
            gstrcpy(&result[i], repositories[index].revisions[i].name);
        return result;
    }
    else { // revision != NULL
        tree_t tree = get_revision_tree(fsinfo, repo, revision);
    #ifdef DEBUG
        printf("[necessary_get_children: retrieved tree %d\n", (int) tree);
    #endif        
        if (!tree)
            return NULL;
        result = gtreecld(tree, internal);
        // free_revision_tree(repo, revision);
        return result;
    }
}

/* private functions */

tree_t get_revision_tree(struct file_system_info *fsinfo, char *repo, char *rev){
    
    revision_t *revisions = NULL;
    int count = 0;
    int i = 0, j = 0;
    char *prefix = NULL;

    printf("get_revision_tree: getting revision tree for %s/%s/\n", repo, rev);
    if (!repo) {
        revisions = repositories[0].revisions;
        count = fsinfo->rev_count[0];
        gmstrcpy(&prefix, "/", rev, 0);
    }
    else {
        if ((i = repo_index(fsinfo, repo)) == -1)
            return NULL;
        revisions = repositories[i].revisions;
        count = fsinfo->rev_count[i];            
        gmstrcpy(&prefix, "/", repo, "/", rev, 0);
    }        
    for (j = 0; j < count; j++)
        if (strcmp(rev, revisions[j].name) == 0)
            break;
    if (j == count) // should never happen
        return NULL;
    if (!revisions[j].tree && build_revision_tree(fsinfo, prefix, revisions, i, j))
        return NULL;
    return revisions[j].tree;
}

/*void free_revision_tree(char *repo, char *rev){

    revision_t *revisions = NULL;
    int count = 0;
    int i = 0, j = 0;

    if (!repo) {
        revisions = repositories[0].revisions;
        count = fsinfo->rev_count[0];
    }
    else {
        for (i = 0; i < fsinfo->repo_count; i++)
            if (strcmp(repo, fsinfo->repos[i]) == 0){
                revisions = repositories[i].revisions;
                count = fsinfo->rev_count[i];
                break;
            }
        if (i == fsinfo->repo_count)
            // should never happen
            return;
    }        
    for (j = 0; j < count; j++)
        if (strcmp(rev, revisions[j].name) == 0)
            break;
    if (j == count)
        // should never happen
        return;
    gtreedel(revisions[j].tree, "/");
    free(revisions[j].tree);
};*/

int build_revision_tree(struct file_system_info *fsinfo, char *prefix, revision_t *revisions, int repo_index, int rev_index){
    
    #define build_revision_tree_finish(value) {             \
        if (current_snapshot){                              \
            unlink(current_snapshot);                       \
            gstrdel(current_snapshot);                      \
        }                                                   \
        return value;                                       \
    }
    
    int snapshot_index = 0;
    char *current_snapshot = NULL;

    printf("[build_revision_tree: building revision tree for index %d\n", rev_index);
    
    if (free_cache())
        build_revision_tree_finish(-1);
    if ((snapshot_index = find_snapshot(revisions, fsinfo->rev_count[repo_index], rev_index)) == -1)
        build_revision_tree_finish(-1);
    printf("%d %d\n", snapshot_index, rev_index);
    if ((current_snapshot = build_snapshot(revisions, rev_index, snapshot_index)) == NULL)
        build_revision_tree_finish(-1);
    if (gtreenew(&(revisions[rev_index].tree)))
        build_revision_tree_finish(-1);
    if (read_revision_necessary(current_snapshot, prefix, revisions[rev_index].tree, fsinfo->rev_count[repo_index] - rev_index - 1))
        build_revision_tree_finish(-1);
    printf("[build_revision_tree: done building\n");
    build_revision_tree_finish(0);
}

int find_snapshot(revision_t *revisions, int count, int rev_index){
    
    int snapshot_index = rev_index;
    char *ext = NULL;

#ifdef DEBUG
    printf("[find_snapshot: finding snapshot for index %d\n", rev_index);
#endif            
    
    while (snapshot_index < count) {
        if ((ext = gpthextptr(revisions[snapshot_index].file)) == NULL)
            return -1;
        if (strcmp(ext, "snapshot") == 0)
            break;
        snapshot_index++;
    }
    if (snapshot_index == count)
        // should never happen
        return -1;
    return snapshot_index;
    
};


int read_revision_necessary(char *snapshot, char *prefix, tree_t tree, int revision){

    #define read_snapshot_finish(value){            \
        if (file)                                   \
            fclose(file);                           \
        return value;                               \
    }

#ifdef DEBUG
    printf("[grdiff.read_snapshot: reading %s\n", snapshot);
#endif            
    FILE *file = NULL;
    stats_t stats;
    
    if ((file = fopen(snapshot, "r")) == NULL)
        read_snapshot_finish(-1);
    memset(&stats, 0, sizeof(stats));
    while (read_stats(&stats, file) == 0){
        gmstrcpy(&stats.path, prefix, "/", stats.internal, 0);
        gstrdel(stats.internal);
        stats.internal = stats.path + strlen(prefix) + strlen("/");        
        stats.rev = revision;
        if (gpthcldptr(&stats.name, stats.path) == -1)
            read_snapshot_finish(-1);
        update_tree(tree, &stats, stats.internal);
        memset(&stats, 0, sizeof(stats));
    }
#ifdef DEBUG
    printf("[grdiff.read_snapshot: done reading snapshot\n");
#endif            
    read_snapshot_finish(0);
	
};

char * build_snapshot(revision_t *revisions, int rev_index, int snapshot_index) {
    
    #define build_snapshot_error {          \
        if (snapshot_desc > 0)              \
            close(snapshot_desc);           \
        if (revision_desc > 0)              \
            close(snapshot_desc);           \
        unlink(temp_snapshot);              \
        gstrdel(temp_snapshot);             \
        return NULL;                        \
    }
    
    char *temp_snapshot = NULL;
    char *snapshot = NULL;
    int i = 0;
    int snapshot_desc = 0;
    int revision_desc = 0;
    
#ifdef DEBUG
    printf("[build_snapshot: building full snapshot for index %d with snapshot %d\n", rev_index, snapshot_index);
#endif
    gmstrcpy(&temp_snapshot, data_dir, "/temp-snapshot-XXXXXX", 0);
    if ((snapshot_desc = mkstemp(temp_snapshot)) == -1)
        build_snapshot_error;
    gmstrcpy(&snapshot, data_dir, "/", revisions[snapshot_index].file, 0);
    
    if ((revision_desc = open(snapshot, O_RDONLY)) == -1)
        build_snapshot_error;
    if (gdesccopy(revision_desc, snapshot_desc))
        build_snapshot_error;
    for (i = snapshot_index - 1; i >= rev_index; i--){
        gmstrcpy(&snapshot, data_dir, "/", revisions[i].file, 0);
        if ((revision_desc = open(snapshot, O_RDONLY)) == -1)
            build_snapshot_error;
        write(snapshot_desc, "\n", 1);
        if (gdesccopy(revision_desc, snapshot_desc))
            build_snapshot_error;
        close(revision_desc);
    }
    close(snapshot_desc);
#ifdef DEBUG
    printf("[build_snapshot: returning temp snapshot %s\n", temp_snapshot);
#endif            
    return temp_snapshot;
    
};

int free_cache(){
    return 0;
}

int repo_exists(struct file_system_info *fsinfo, char *repo_name){
    return repo_index(fsinfo, repo_name) != -1;
};

int revision_exists(struct file_system_info *fsinfo, char *repo_name, char *revision_name){
    
    revision_t *revisions = NULL;
    int i = 0;
    int count = 0;
    
    if (repo_name == NULL){
        revisions = repositories[0].revisions;
        count = fsinfo->rev_count[0];
    }
    else{
        if ((i = repo_index(fsinfo, repo_name)) == -1)
            return 0;
        revisions = repositories[i].revisions;
        count = fsinfo->rev_count[i];
    };
    for (i = 0; i < count; i++)
        if (strcmp(revisions[i].name, revision_name) == 0)
            return 1;
    return 0;
    
};

int repo_index(struct file_system_info *fsinfo, char *repo){

    int i = 0;
    for (; i < fsinfo->repo_count && strcmp(repositories[i].name, repo) != 0; i++);
    if (i == fsinfo->repo_count) // failed to find repo
        return -1;
    return i;

};
