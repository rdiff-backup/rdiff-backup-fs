#include "headers.h"
#include "externs.h"
#include "parse.h"
#include "initialize.h"

int repo_count = 0;
int *rev_count = NULL;
char **repos = NULL;
char **repo_names = NULL;

char *mount = NULL;
char *tmp_dir = NULL;
char *tmp_file = NULL;

int structure = STRUCTURE_FULL;
int layout = LAYOUT_ALL;

struct fuse_operations operations;

int run(int argc, char **argv){

    char *args[3];

    parse(argc, argv);
    initialize();
    
    args[0] = argv[0];
    args[1] = mount;
    args[2] = "-d";

    return fuse_main(3, args, &operations, NULL);

};

int main(int argc, char** argv){
      
    return run(argc, argv);
	
};
