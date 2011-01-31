#include "headers.h"
#include "externs.h"
#include "parse.h"
#include "initialize.h"

char *mount = NULL;
char *tmp_dir = NULL;
char *data_dir = NULL;

int structure = STRUCTURE_NECESSARY;
int layout = LAYOUT_ALL;
int debug = 0;

struct fuse_operations operations;
struct file_system_info *file_system_info;

int run(int argc, char **argv){

    char *args[3];
    
    file_system_info = single(struct file_system_info);

    parse(file_system_info, argc, argv);
    initialize(file_system_info);
    
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
