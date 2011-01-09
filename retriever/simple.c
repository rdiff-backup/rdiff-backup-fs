#include "simple.h"
#include "support.h"
#include "support/gutils.h"

int __release_simple(struct stats *, int);

// public:

int retrieve_simple(struct file_system_info *fsinfo, struct stats *stats){

	int repo = 0;
	
	debug(3, "[Function: retrieve_simple] Retrieving file %s;\n", stats->path);
	if ((repo = repo_number(fsinfo, stats)) == -1)
		return -1;
    return retrieve_common(fsinfo, stats, repo);
		
};

int release_simple(struct file_system_info *fsinfo, struct stats *stats){

	int repo = 0;
	
	debug(3, "[Function: release_simple] Retrieving file %s from revision %dB;\n", stats->path, stats->rev);
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
	stats->shared--;
	if (stats->shared == 0){
		unlink(stats->tmp_path);
		gstrdel(stats->tmp_path);
	};
	__release_simple_finish(0);

};
