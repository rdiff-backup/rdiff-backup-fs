#include "necessary.h"

int necessary_limit = DEFAULT_NECESSARY_LIMIT;

struct revision {
	char *name;
	struct node *tree;
};

struct repo {
    char *name;
    struct revision *revisions;
};

static struct node * get_revision_tree(char *repo, char *revision);

static int build_revision_tree(struct repo *repo, struct revision *revision);

static int free_cache();

struct repo *repositories = NULL;

int necessary_build(char *repo){

    #define necessary_build_finish(value) {                     \
        gstrdel(path);                                          \
        if (revs){                                              \
            for (i = 0; i < rev_count[0]; i++)                  \
                if (revs[i])                                    \
                    free(revs[i]);                              \
            free(revs);                                         \
        }                                                       \
        return value;                                           \
    }

    char *path = NULL;
    char **revs = NULL;
    int i = 0;

	if (gmstrcpy(&path, repo, "/rdiff-backup-data", 0) != 0)
		necessary_build_finish(-1);	
	if ((rev_count = single(int)) == NULL)
		necessary_build_finish(-1);
    if ((rev_count[0] = unzip_revs(path)) == -1)
    	necessary_build_finish(-1);
    if ((revs = calloc(rev_count[0], sizeof(char *))) == NULL)
    	necessary_build_finish(-1);
    if (get_revisions(rev_count[0], revs) == -1)
    	necessary_build_finish(-1);
    gstrsort(revs, rev_count[0]);
    if ((repositories = single(struct repo)) == NULL)
        necessary_build_finish(-1);
    repositories[0].revisions = calloc(rev_count[0], sizeof(struct revision));
    for (i = 0; i < rev_count[0]; i++)
        gstrcpy(&repositories[0].revisions[i].name, revs[i]);
	necessary_build_finish(0);
}

int necessary_build_multi(int count, char **repo){
	return 0;
}

int necessary_get_file(char *repo, char *revision, char *internal, 
					   struct stats **stats){
    struct node *tree = get_revision_tree(repo, revision);
    if (!tree)
        return -1;
    return gtreeget(tree, internal, stats);    
}

char** necessary_get_children(char *repo, char *revision, char *internal){
    struct node *tree = get_revision_tree(repo, revision);
    if (!tree)
        return NULL;
    return gtreecld(tree, internal);
}

struct node * get_revision_tree(char *repo, char *rev){
    
    struct revision *revisions = NULL;
    int count = 0;
    int i = 0, j = 0;
    
    if (!repo) {
        revisions = repositories[0].revisions;
        count = rev_count[0];
    }
    else {
        for (i = 0; i < repo_count; i++)
            if (strcmp(repo, repos[i]) == 0){
                revisions = repositories[i].revisions;
                count = rev_count[i];
                break;
            }
        if (i == repo_count)
            // should never happen
            return NULL;
    }        
    for (j = 0; j < count; j++)
        if (strcmp(rev, revisions[j].name) == 0)
            break;
    if (j == count)
        // should never happen
        return NULL;
    if (!revisions[j].tree)
        build_revision_tree(&repositories[i], &revisions[j]);
    return revisions[j].tree;
}

int build_revision_tree(struct repo *repo, struct revision *revision){
    if (free_cache())
        return -1;
    return 0;
}

int free_cache(){
    return 0;
}
