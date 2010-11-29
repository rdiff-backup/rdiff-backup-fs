#include "headers.h"
#include "externs.h"
#include "parse.h"
#include "initialize.h"

// number of repositories
int repo_count = 0;
// number of revisions in every repository
int *rev_count = NULL;
// paths to repositories
char **repos = NULL;
// names of repositories
char **repo_names = NULL;

char *mount = NULL;
char *tmp_dir = NULL;
char *tmp_file = NULL;

int structure = STRUCTURE_FULL;
int layout = LAYOUT_ALL;
int debug = 0;

struct fuse_operations operations;

int run(int argc, char **argv){

    char *args[3];

    parse(argc, argv);
    initialize();
    
    args[0] = argv[0];
    args[1] = mount;
    args[2] = "-d";
    
    if (debug)
        return fuse_main(3, args, &operations, NULL);
    else
        return fuse_main(2, args, &operations, NULL);

};

int main(int argc, char** argv){
      
    return run(argc, argv);
	
};
