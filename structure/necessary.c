#include "sglib.h"

#include "necessary.h"
#include "../headers.h"
#include "../support/gutils.h"

int necessary_limit = DEFAULT_NECESSARY_LIMIT;

struct revision {
	char *name;
    char *file;
	tree_t tree;
};

typedef struct revision revision_t;

struct repository {
    struct revision *revisions;
};

typedef struct repository repository_t;

repository_t *repositories = NULL;
static struct stats root;

typedef struct stats_list {
    stats_t *stats;
    struct stats_list *next;
} stats_list_t;

/* private functions' prototypes */

static struct node * get_revision_tree(struct file_system_info *, char *, char *);

static int build_revision_tree(struct file_system_info *fsinfo, char *, revision_t *, int, int);

static void free_revision_tree(struct file_system_info *, char *repo, char *revision);

static int find_snapshot(revision_t *, int, int);

static char * build_snapshot(revision_t *, int, int);

int read_revision_necessary(char *, char *, tree_t, int);

/*
 * retrieves index of a repo from name
 */
 static int repository_index(struct file_system_info *, char *);
 
 static int revision_index(struct file_system_info *, char *, char *);

/*
 * checks whether repo with given name exists
 */
static int repository_exists(struct file_system_info *, char *repo_name);

/*
 * checks whether revision in given repo exists
 */
static int revision_exists(struct file_system_info *, char *repo_name, char *revision_name);

static void find_open(node_t *, stats_list_t *);

#define STATS_LIST_COMPARATOR(first, second) (strcmp(first->stats->path, second->stats->path))

SGLIB_DEFINE_LIST_PROTOTYPES(stats_list_t, STATS_LIST_COMPARATOR, next);
SGLIB_DEFINE_LIST_FUNCTIONS(stats_list_t, STATS_LIST_COMPARATOR, next);

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

    debug(1, "checking file %s/%s/%s\n", repo, revision, internal);
    if (revision == NULL && (repo == NULL || repository_exists(fsinfo, repo))){
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
        int result = gtreeget(tree, internal, stats);
        free_revision_tree(fsinfo, repo, revision);
        return result;
    }
}

char** necessary_get_children(struct file_system_info *fsinfo, char *repo, char *revision, char *internal){
    
    int i = 0, index = 0;
    char **result = 0;

    debug(1, "getting children of %s/%s/%s\n", repo, revision, internal);
    if (revision != NULL && repo == NULL && fsinfo->repo_count > 1)
        return NULL; // should not happen
    if (revision == NULL && repo == NULL && fsinfo->repo_count > 1){
        result = calloc(fsinfo->repo_count + 1, sizeof(char *));
        for (i = 0; i < fsinfo->repo_count; i++)
            gstrcpy(&result[i], fsinfo->repo_names[i]);
        return result;
    }
    else if (revision == NULL && (fsinfo->repo_count == 1 || repo != NULL)){
        index = 0;
        if (repo && ((index = repository_index(fsinfo, repo)) == -1))
            return NULL;
        result = calloc(fsinfo->rev_count[index] + 1, sizeof(char *));
        for (i = 0; i < fsinfo->rev_count[index]; i++)
            gstrcpy(&result[i], repositories[index].revisions[i].name);
        return result;
    }
    else { // revision != NULL
        tree_t tree = get_revision_tree(fsinfo, repo, revision);
        debug(1, "retrieved tree %d\n", (int) tree);
        if (!tree)
            return NULL;
        result = gtreecld(tree, internal);
        free_revision_tree(fsinfo, repo, revision);
        return result;
    }
}

/* private functions */

int build_revision_tree(struct file_system_info *fsinfo, char *prefix, revision_t *revisions, int repository_index, int rev_index){
    
    #define build_revision_tree_finish(value) {             \
        if (current_snapshot){                              \
            unlink(current_snapshot);                       \
            gstrdel(current_snapshot);                      \
        }                                                   \
        return value;                                       \
    }
    
    int snapshot_index = 0;
    char *current_snapshot = NULL;

    debug(2, "building revision tree for index %d\n", rev_index);
    
    if ((snapshot_index = find_snapshot(revisions, fsinfo->rev_count[repository_index], rev_index)) == -1)
        build_revision_tree_finish(-1);
    if ((current_snapshot = build_snapshot(revisions, rev_index, snapshot_index)) == NULL)
        build_revision_tree_finish(-1);
    if (gtreenew(&(revisions[rev_index].tree)))
        build_revision_tree_finish(-1);
    if (read_revision_necessary(current_snapshot, prefix, revisions[rev_index].tree, fsinfo->rev_count[repository_index] - rev_index - 1))
        build_revision_tree_finish(-1);
    debug(1, "done building\n");
    build_revision_tree_finish(0);
}

int find_snapshot(revision_t *revisions, int count, int rev_index){
    
    int snapshot_index = rev_index;
    char *ext = NULL;

    debug(2, "finding snapshot for index %d\n", rev_index);    
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

tree_t get_revision_tree(struct file_system_info *fsinfo, char *repo, char *rev){
    
    revision_t *revisions = NULL;
    int count = 0;
    int i = 0, j = 0;
    char *prefix = NULL;

    debug(2, "getting revision tree for %s/%s/\n", repo, rev);
    if (!repo) {
        revisions = repositories[0].revisions;
        count = fsinfo->rev_count[0];
        gmstrcpy(&prefix, "/", rev, 0);
    }
    else {
        if ((i = repository_index(fsinfo, repo)) == -1)
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

void free_revision_tree(struct file_system_info *fsinfo, char *repo_name, char *rev_name){
    
    int rev_index = 0, repo_index = 0;
    revision_t *rev = NULL;
    
    debug(2, "freeing revision tree for %s/%s\n", repo_name, rev_name);
    
    if ((repo_index = repository_index(fsinfo, repo_name)) == -1)
        return;
    if ((rev_index = revision_index(fsinfo, repo_name, rev_name)) == -1)
        return;
    rev = &repositories[repo_index].revisions[rev_index];

    find_open(rev->tree, NULL);
    gtreedel(rev->tree, "/");
    free(rev->tree);
    rev->tree = NULL;
    debug(2, "revision tree deleted\n");
};

void find_open(node_t *node, stats_list_t *list){
    int i = 0;
    
    if (node->stats->shared > 0);
        // add to list
    for (i = 0; i < node->size; i++)
        find_open(node->children[i], list);
};

int read_revision_necessary(char *snapshot, char *prefix, tree_t tree, int revision){

    #define read_snapshot_finish(value){            \
        if (file)                                   \
            fclose(file);                           \
        return value;                               \
    }

    debug(2, "reading %s\n", snapshot);
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
    debug(1, "done reading snapshot\n");
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
    
    debug(2, "building full snapshot for index %d with snapshot %d\n", rev_index, snapshot_index);
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
    debug(2, "returning temp snapshot %s\n", temp_snapshot);
    return temp_snapshot;
};

int repository_exists(struct file_system_info *fsinfo, char *repo_name){
    return repository_index(fsinfo, repo_name) != -1;
};

int revision_exists(struct file_system_info *fsinfo, char *repo_name, char *revision_name){
    return revision_index(fsinfo, repo_name, revision_name) != -1;
};

int revision_index(struct file_system_info *fsinfo, char *repo_name, char *revision_name){

    revision_t *revisions = NULL;
    int i = 0;
    int count = 0;

    if ((i = repository_index(fsinfo, repo_name)) == -1)
        return -1;    
    revisions = repositories[i].revisions;
    count = fsinfo->rev_count[i];
    for (i = 0; i < count; i++)
        if (strcmp(revisions[i].name, revision_name) == 0)
            return 1;
    return -1;
    
};

int repository_index(struct file_system_info *fsinfo, char *repo){
    int i = 0;
    if (fsinfo->repo_count == 1)
        return 0;
    for (; i < fsinfo->repo_count && strcmp(fsinfo->repo_names[i], repo) != 0; i++);
    if (i == fsinfo->repo_count) // failed to find repo
        return -1;
    return i;
};
