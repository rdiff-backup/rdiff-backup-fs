#include <pthread.h>
#include <assert.h>

#include "necessary.h"
#include "headers.h"
#include "support/gutils.h"

struct revision {
    pthread_mutex_t mutex;
    struct stats stats;
	char *name;
    char *file;
	tree_t tree;
};

typedef struct revision revision_t;

struct repository {
    struct revision *revisions;
    struct stats stats;
};

typedef struct repository repository_t;

repository_t *repositories = NULL;
static struct stats root;

struct cache_node;

typedef struct cache_node cache_node_t;

struct cache_node {
    int repo;
    int rev;
    cache_node_t *next;
    cache_node_t *prev;
};

struct cache_list {
    cache_node_t *head;
    cache_node_t *tail;
    int size;
    pthread_mutex_t mutex;
};

typedef struct cache_list cache_list_t;

static cache_list_t cache_list;

int necessary_limit = DEFAULT_NECESSARY_LIMIT;

/* private functions' prototypes */

static struct node * get_revision_tree(struct file_system_info *, char *, char *);

static int build_revision_tree(struct file_system_info *fsinfo, char *, revision_t *, int, int);

static void free_revision_tree(int repo, int rev);

static int find_snapshot(revision_t *, int, int);

static char * build_snapshot(struct file_system_info *, revision_t *, int, int, int);

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

static int add_cached_tree(cache_list_t *list, int repo, int rev);

static void free_cached_tree(cache_list_t *list);

/* public functions */

int necessary_build(struct file_system_info *fsinfo){

    int i = 0;
    
    if ((fsinfo->rev_count[0] = gather_revisions(fsinfo, fsinfo->repos[0], data_dir)) == -1)
        return -1;
    if ((repositories = single(repository_t)) == NULL)
        return -1;
    cache_list.head = cache_list.tail = NULL;
    cache_list.size = 0;
    pthread_mutex_init(&cache_list.mutex, 0);
    repositories[0].revisions = calloc(fsinfo->rev_count[0], sizeof(revision_t));
    for (i = 0; i < fsinfo->rev_count[0]; i++) {
        repositories[0].revisions[i].name = get_revs_dir(fsinfo, fsinfo->revs[i]);
        gstrcpy(&repositories[0].revisions[i].file, fsinfo->revs[i]);
        pthread_mutex_init(&repositories[0].revisions[i].mutex, 0);
        set_directory_stats(&repositories[0].revisions[i].stats);
    }
    set_directory_stats(&root);
    root.nlink = hardlinkcount(fsinfo->rev_count[0]);
	return 0;
}

int necessary_build_multi(struct file_system_info *fsinfo){
    
    int i = 0, j = 0;

    cache_list.head = cache_list.tail = NULL;
    cache_list.size = 0;
    pthread_mutex_init(&cache_list.mutex, 0);    
    if ((repositories = calloc(fsinfo->repo_count, sizeof(revision_t))) == NULL)
        return -1;
    for (i = 0; i < fsinfo->repo_count; i++){
        char *target_dir = NULL;
        gmstrcpy(&target_dir, data_dir, "/", fsinfo->repo_names[i], 0);
        if (mkdir(target_dir, S_IRWXU))
            return -1;
        if ((fsinfo->rev_count[i] = gather_revisions(fsinfo, fsinfo->repos[i], target_dir)) == -1)
            return -1;
        repositories[i].revisions = calloc(fsinfo->rev_count[i], sizeof(revision_t));
        for (j = 0; j < fsinfo->rev_count[i]; j++){
            repositories[i].revisions[j].name = get_revs_dir(fsinfo, fsinfo->revs[j]);
            set_directory_stats(&repositories[i].revisions[j].stats);
            gstrcpy(&repositories[i].revisions[j].file, fsinfo->revs[j]);
        }
        gstrdel(target_dir);
        set_directory_stats(&repositories[i].stats);
        repositories[i].stats.nlink = hardlinkcount(fsinfo->rev_count[i]);
    }
    set_directory_stats(&root);
    root.nlink = hardlinkcount(fsinfo->repo_count);
    // TODO: check why this is -1 and why it is not considered a failure
    return -1;
}

int necessary_get_file(struct file_system_info *fsinfo, char *repo, char *revision, char *internal, 
					   stats_t **stats){

    if (revision == NULL && repo == NULL)
        copy_stats(&root, stats);
    else if (revision == NULL && repo != NULL && repository_exists(fsinfo, repo)){
        int index = repository_index(fsinfo, repo);
        copy_stats(&repositories[index].stats, stats);
    }
    else if (revision != NULL && internal == NULL){
        int repo_index = repository_index(fsinfo, repo);
        if (!revision_exists(fsinfo, repo, revision))
            return -1;
        else {
            int rev_index = revision_index(fsinfo, repo, revision);
            copy_stats(&repositories[repo_index].revisions[rev_index].stats, stats);
        }
    }
    else{
        int repo_index = repository_index(fsinfo, repo);
        int rev_index = revision_index(fsinfo, repo, revision);
        if (repo_index == -1 || rev_index == -1)
            return -1;
        lock(repositories[repo_index].revisions[rev_index].mutex);
        struct node *tree = get_revision_tree(fsinfo, repo, revision);
        if (!tree) {
            unlock(repositories[repo_index].revisions[rev_index].mutex);
            return -1;
        }
        gtreenlinks(tree);
        // only after tree is retrieved for the first time, we can update nlink count
        repositories[repo_index].revisions[rev_index].stats.nlink = tree->stats->nlink;
        int result = gtreeget(tree, internal, stats);
        unlock(repositories[repo_index].revisions[rev_index].mutex);
        return result;
    }
    return 0;
}

char** necessary_get_children(struct file_system_info *fsinfo, char *repo, char *revision, char *internal){
    
    int i = 0, index = 0;
    char **result = 0;

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
        int repo_index = repository_index(fsinfo, repo);
        int rev_index = revision_index(fsinfo, repo, revision);
        if (repo_index == -1 || rev_index == -1)
            return NULL;
        lock(repositories[repo_index].revisions[rev_index].mutex);
        tree_t tree = get_revision_tree(fsinfo, repo, revision);
        if (!tree) {
            unlock(repositories[repo_index].revisions[rev_index].mutex);
            return NULL;
        }
        result = gtreecld(tree, internal);
        unlock(repositories[repo_index].revisions[rev_index].mutex);
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
    if ((current_snapshot = build_snapshot(fsinfo, revisions, repository_index, rev_index, snapshot_index)) == NULL)
        build_revision_tree_finish(-1);
    if (gtreenew(&(revisions[rev_index].tree)))
        build_revision_tree_finish(-1);
    if (read_revision_necessary(current_snapshot, prefix, revisions[rev_index].tree, fsinfo->rev_count[repository_index] - rev_index - 1))
        build_revision_tree_finish(-1);
    add_cached_tree(&cache_list, repository_index, rev_index);
    debug(2, "done building\n");
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
    int i = 0, j = 0;
    char *prefix = NULL;

    debug(2, "getting revision tree for %s/%s/\n", repo, rev);
    if (!repo) {
        revisions = repositories[0].revisions;
        gmstrcpy(&prefix, "/", rev, 0);
    }
    else {
        if ((i = repository_index(fsinfo, repo)) == -1)
            return NULL;
        revisions = repositories[i].revisions;
        gmstrcpy(&prefix, "/", repo, "/", rev, 0);
    }
    if ((j = revision_index(fsinfo, repo, rev)) == -1)
        return NULL;
    debug(3, "repo: %s = %d, revision: %s = %d\n", repo, i, rev, j);
    int result = 0;
    if (!revisions[j].tree)
        result = build_revision_tree(fsinfo, prefix, revisions, i, j);
    gstrdel(prefix);
    if (result == -1)
        return NULL;
    return revisions[j].tree;
}

void free_revision_tree(int repo_index, int rev_index){
    
    revision_t *rev = NULL;
    
    debug(2, "freeing revision tree for %d/%d\n", repo_index, rev_index);
    rev = &repositories[repo_index].revisions[rev_index];
    gtreedel(rev->tree, "/");
    rev->tree = NULL;
    debug(2, "revision tree deleted\n");
};

int read_revision_necessary(char *snapshot, char *prefix, tree_t tree, int revision){

    #define read_snapshot_finish(value){            \
        if (file)                                   \
            fclose(file);                           \
        return value;                               \
    }

    debug(2, "reading %s with revision %d\n", snapshot, revision);
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

char * build_snapshot(struct file_system_info *fsinfo, revision_t *revisions, int repo_index, int rev_index, int snapshot_index) {
    
    #define build_snapshot_error {          \
        if (snapshot_desc > 0)              \
            close(snapshot_desc);           \
        if (revision_desc > 0)              \
            close(snapshot_desc);           \
        unlink(temp_snapshot);              \
        gstrdel(snapshot);                  \
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
    if (fsinfo->repo_count == 1)
        gmstrcpy(&snapshot, data_dir, "/", revisions[snapshot_index].file, 0);
    else
        gmstrcpy(&snapshot, data_dir, "/", fsinfo->repo_names[repo_index], "/", revisions[snapshot_index].file, 0);  
    debug(2, "%s\n", snapshot);
    if ((revision_desc = open(snapshot, O_RDONLY)) == -1)
        build_snapshot_error;
    if (gdesccopy(revision_desc, snapshot_desc))
        build_snapshot_error;
    gstrdel(snapshot);
    close(revision_desc);
    for (i = snapshot_index - 1; i >= rev_index; i--){
        if (fsinfo->repo_count == 1)
            gmstrcpy(&snapshot, data_dir, "/", revisions[i].file, 0);
        else
            gmstrcpy(&snapshot, data_dir, "/", fsinfo->repo_names[repo_index], "/", revisions[i].file, 0);  
        if ((revision_desc = open(snapshot, O_RDONLY)) == -1)
            build_snapshot_error;
        if (write(snapshot_desc, "\n", 1) == -1)
            build_snapshot_error;
        if (gdesccopy(revision_desc, snapshot_desc))
            build_snapshot_error;
        gstrdel(snapshot);
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
            return i;
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

int add_cached_tree(cache_list_t *list, int repo, int rev){
    
    assert(necessary_limit > 0);
    
    lock(list->mutex);
    cache_node_t *node = single(cache_node_t);
    node->repo = repo;
    node->rev = rev;
    node->next = list->head;
    if (list->head)
        list->head->prev = node;
    if (!list->tail)
        list->tail = node;
    list->head = node;
    list->size++;
    if (list->size > necessary_limit)
        free_cached_tree(list);
    unlock(list->mutex);
    return 0;
};

void free_cached_tree(cache_list_t *list){
    
    assert(list->size > necessary_limit && necessary_limit > 0);
    debug(3, "freeing cache for repo: %d and rev: %d\n", list->tail->repo, list->tail->rev);
    
    cache_node_t *node;
    
    list->tail->prev->next = NULL;
    lock(repositories[list->tail->repo].revisions[list->tail->rev].mutex);
    free_revision_tree(list->tail->repo, list->tail->rev);
    unlock(repositories[list->tail->repo].revisions[list->tail->rev].mutex);
    node = list->tail;
    list->tail = list->tail->prev;
    list->size--;
    free(node);
};
