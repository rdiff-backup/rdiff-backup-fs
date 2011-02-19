#include <fcntl.h>

#include "gtree.h"
#include "gutils.h"

// prototypes

struct node* find_node(struct node *, const char *);

struct node* find_parent(struct node *, const char *);

/*
 *
 */
int tree_add_node(struct node *, char *);

int delete_node(struct node *);

/*
 * increases capacity of an array holding pointers to the children of this node
 */
int tree_increase_capacity(struct node *);

int tree_save_node(struct node *, int);

// public

int gtreenew(tree_t *tree){

    struct stats *stats = single(struct stats);
	
	(*tree) = single(struct node);
	if ((*tree) == NULL)
		return -1;
	if (gstrcpy(&stats->path, "/")){
		free(*tree);
		return -1;
	};
    stats->name = stats->path;
    stats->internal = NULL;
    stats->rev = -1;
    set_directory_stats(stats);
    (*tree)->stats = stats;
    return 0;
	
};

int gtreeadd(struct node *tree, struct stats *stats, char *path){
	
    struct node *node = tree, *next = NULL;
    char **parts = NULL;
    int count = gpthdiv(path, &parts);
    int i = 0, j = 0;

	debug(3,"Received file on path %s;\n", path);
    if (count == 0)
		return -1;
    for (i = 0; i < count; i++){
		next = NULL;
		for (j = 0; (j < node->size) && (next == NULL); j++)
	    	if (strcmp(node->children[j]->name, parts[i]) == 0)
				next = node->children[j];
		if (next == NULL){
	    	tree_add_node(node, parts[i]);
	    	next = node->children[node->size - 1];
		};
		node = next;
    };
    for (i = 0; i < count; i++)
        free(parts[i]);
    free(parts);
    if (node->stats == NULL)
    	node->stats = single(struct stats);
    else // we will substitute old stats in this node, so we need free old path
        gstrdel(node->stats->path);
    memcpy(node->stats, stats, sizeof(struct stats));
    stats->path = NULL;
    stats->name = NULL;
    stats->internal = NULL;
    return 0;

};

char** gtreecld(struct node *tree, const char *path){

	char **results = NULL;
	int i = 0;
	struct node *node = find_node(tree, path);

	debug(3, "Received path %s;\n", path);
	if (node == NULL)
		return NULL;
	results = calloc(node->size + 1, sizeof(char *));
	for (i = 0; i < node->size; i++)
		gstrcpy(&results[i], node->children[i]->name);
	return results;
	
};

int gtreeget(struct node *tree, const char *path, struct stats **stats){

    debug(3, "Getting stats for %s;\n", path);    
	struct node *node = find_node(tree, path);
	if (node == NULL)
		return -1;
    debug(3, "found node; copying stats;\n");
	copy_stats(node->stats, stats);
	return 0;

};

int gtreedel(struct node *tree, const char *path){

	struct node *parent = NULL;
	struct node *node = NULL;
	int i = 0;

	// printf("[Function: tree_delete] Deleting path %s;\n", path);
	if ((node = find_node(tree, path)) == NULL)
		return -1;
    parent = find_parent(tree, path);
	if (parent){
        for (i = 0; (i < parent->size) && (parent->children[i] != node); i++);
        for (;i < parent->size - 1; i++)
            parent->children[i] = parent->children[i + 1];
        parent->size--;
    }
	return delete_node(node);

};

int gtreesave(struct node *tree, char *path){
	
	int file =  0;
	
	if ((file = open(path, O_WRONLY | O_CREAT, S_IRWXU)) == -1)
		return -1;
	tree_save_node(tree, file);
	close(file);
	return 0;
	
};

void gtreenlinks(tree_t tree){
    
};

// private:

struct node * find_node(struct node *root, const char *path){

    #define find_node_finish(value) {           \
        for (i = 0; i < count; i++)             \
            gstrdel(parts[i]);                  \
        free(parts);                            \
        return value;                           \
    }

	char **parts = NULL;
	int count = gpthdiv(path, &parts);
	struct node *node = root, *next = NULL;
	int i = 0, j = 0;

	// printf("[Function: find_node] Finding node with path %s;\n", path);
	for (i = 0; i < count; i++){
		for (j = 0; (j < node->size) && (next == NULL); j++)
			if (strcmp(node->children[j]->name, parts[i]) == 0)
				next = node->children[j];
		if (next == NULL)
			find_node_finish(NULL);
		node = next;
		next = NULL;
	};
    find_node_finish(node);

};

struct node* find_parent(struct node *root, const char *path){

	char *__path = NULL;
	int length = 0;
	int __length = 0;
	struct node *node = NULL;
	int i = 0;

	if (gpthdpt(path) <= 0)
		return NULL;
	length = strlen(path);
	gpthcln(path, __path, length, __length);
	for (i = __length - 1; (i >= 0) && (__path[i] != '/'); i--)
		__path[i] = 0;
	__path[i] = 0;
	node = find_node(root, __path);
	gstrdel(__path);
	return node;
	
};

int tree_add_node(struct node *node, char *name){

	// printf("[Function: tree_add_node] Adding file %s;\n", name);
    if (node->size == node->capacity)
		tree_increase_capacity(node);
    node->children[node->size] = calloc(1, sizeof(struct node));
    gstrcpy(&node->children[node->size]->name, name);
    node->size++;
    return 0;
    
};

int tree_increase_capacity(struct node *node){
    
    int i = 0;
    struct node **temp = NULL;

    if (node->capacity == 0){
		node->capacity = 1;
		node->children = calloc(1, sizeof(struct node *));
		return 0;
    }
    temp = calloc(node->size, sizeof(struct node *));
    for (i = 0; i < node->size; i++)
		temp[i] = node->children[i];
    free(node->children);
    node->capacity *= 2;
    node->children = calloc(node->capacity, sizeof(struct node));
    for (i = 0; i < node->size; i++)
		node->children[i] = temp[i];
    free(temp);
    return 0;
    
};

int delete_node(struct node *node){

	int i = 0;
    //printf("delete_node: completely deleting node with path %s\n", node->stats->path);

	for (i = 0; i < node->size; i++)
		delete_node(node->children[i]);
    free(node->name);
    free(node->children);
    free(node->stats->path);
    free(node->stats);
	free(node);

	return 0;

};

/*
 * saves node information to an open file in fashion similar to 
 * rdiff-backup mirror_metadata files; afterwards saves all its children
 * 
 * @1: node to be saved
 * @2: descriptor of an open file
 * 
 * returns: 0 on a success, -1 otherwise
 */
int tree_save_node(struct node *node, int desc){
	
	#define tree_save_node_finish(value) {			\
		gstrdel(temp);								\
		gstrdel(result);							\
		return value;								\
	}
	
	char *temp = NULL;
	char *result = NULL;
	int i = 0;
	
	// setting file path
	asprintf(&temp, "File %s\n", node->stats->internal);
	gstrcat(&result, temp);
	gstrdel(temp);
	
	// setting file type
	if (node->stats->type == S_IFDIR)
		gstrcat(&result, "  Type dir\n");
	else if (node->stats->type == S_IFREG)
		gstrcat(&result, "  Type reg\n");
	else if (node->stats->type == S_IFLNK)
		gstrcat(&result, "  Type lnk\n");
	else
		tree_save_node_finish(-1);
	
	// setting modification time
	asprintf(&temp, "  ModTime %u\n", (unsigned) node->stats->ctime);
	gstrcat(&result, temp);
	gstrdel(temp);
	
	// setting file size
	if (node->stats->type == S_IFREG){
		asprintf(&temp, "Size %lld\n", node->stats->size);
		gstrcat(&result, temp);
		gstrdel(temp);
	}
	
	write(desc, result, strlen(result));
	for (i = 0; i < node->size; i++)
		tree_save_node(node->children[i], desc);
	tree_save_node_finish(0);
		
};
