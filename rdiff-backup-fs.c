#include "headers.h"
#include "externs.h"
#include "parse.h"
#include "initialize.h"

#define FUSE_USE_VERSION 26

#include <fuse.h>

char *mount = NULL;
char *tmp_dir = NULL;
char *data_dir = NULL;

int structure = STRUCTURE_NECESSARY;
int layout = LAYOUT_ALL;
int debug = 0;

char *fuse_options[MAX_FUSE_OPTIONS];
int fuse_options_size = 0;

struct fuse_operations operations;
struct file_system_info *file_system_info;

int run(int argc, char **argv){

    char **args;
    int i = 0, options_count = 0;
    
    file_system_info = single(struct file_system_info);

    parse(file_system_info, argc, argv);
    initialize(file_system_info);
    
    args = calloc(3 + fuse_options_size, sizeof(char *));
    args[0] = argv[0];
    args[1] = mount;
    options_count = 2;
    if (debug){
        args[2] = "-d";
        options_count++;
    }
    for (i = 0; i < fuse_options_size; options_count++, i++)
        gmstrcpy(&args[options_count], "-o", fuse_options[i], 0);
    
    return fuse_main(options_count, args, &operations, NULL);

};

int main(int argc, char** argv){
      
    return run(argc, argv);
	
};
