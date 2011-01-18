#include "simple.h"
#include "support.h"
#include "support/gutils.h"

int __release_simple(struct stats *, int);

// public:

int retrieve_simple(struct file_system_info *fsinfo, struct stats *stats){

	int repo = 0;
	
	debug(2, "Retrieving file %s;\n", stats->path);
	if ((repo = repo_number(fsinfo, stats)) == -1)
		return -1;
    return retrieve_common(fsinfo, stats, repo);
		
};

int release_simple(struct file_system_info *fsinfo, struct stats *stats){

	int repo = 0;
	
	debug(2, "Retrieving file %s from revision %dB;\n", stats->path, stats->rev);
	if ((repo = repo_number(fsinfo, stats)) == -1)
		return -1;
	return __release_simple(stats, repo);
		
};

// private:

int __release_simple(struct stats *stats, int index){

#define __release_simple_finish(value){						\
			unlock(file_mutex[index][stats->rev]);			\
			return value;									\
		}

	lock(file_mutex[index][stats->rev]);
    node_t *node = get_open_file(stats->path);
    if (!node)
        return -1;
	node->count--;
	if (node->count == 0){
		unlink(node->tmp_path);
        delete_open_file(node);
	};
	__release_simple_finish(0);

};
