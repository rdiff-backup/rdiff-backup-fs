#include "limit.h"
#include "support/gutils.h"
#include "support.h"

extern int cache_limit;
// count of all opened files
int open_count = 0;
// count of all cached files
int cache_count = 0;
// open and cached file count synchronisation
pthread_mutex_t cache_mutex = PTHREAD_MUTEX_INITIALIZER; 
// retrieved files still cached on the disk
struct cache *cache = NULL;
struct cache *last = NULL;

int __retrieve_limit(struct file_system_info *fsinfo, struct stats *, int);
int __release_limit(struct stats *, int);

struct cache {
	node_t *node;
	struct cache *next;
};

int cache_add(node_t *);
int cache_delete();

// public:

int retrieve_limit(struct file_system_info *fsinfo, struct stats *stats){

	int repo = 0;

	debug(3, "Retrieving file; currently %d open and %d cached files;\n", 
		   open_count, cache_count);
	if ((repo = repo_number(fsinfo, stats->path)) == -1)
		return -1;
	return __retrieve_limit(fsinfo, stats, repo);

};

int release_limit(struct file_system_info *fsinfo, struct stats *stats){

	int repo = 0;

	debug(3, "Releasing file; currently %d open and %d cached files;\n", 
		   open_count, cache_count);
	if ((repo == repo_number(fsinfo, stats->path)) == -1)
		return -1;
	return __release_limit(stats, repo);
		
};

// private:

int __retrieve_limit(struct file_system_info *fsinfo, struct stats *stats, int repo){

#define __retrieve_limit_finish(value){						\
			unlock(file_mutex[repo][stats->rev]);			\
			gstrdel(file);									\
			gstrdel(revision);								\
			return value;									\
		}

	char *file = NULL;
	char *revision = calloc(20, sizeof(char));
	struct stat temp;
    node_t *node;

	lock(file_mutex[repo][stats->rev]);
    if ((node = add_file(open_files, stats->path, stats->rev)) == NULL)
        __retrieve_limit_finish(-1);
	if (node->count > 0){
		node->count++;
		__retrieve_limit_finish(0);
	};

	// retrieving file
	if (create_tmp_file(stats, node) == -1)
		__retrieve_limit_finish(-1);
	if (gmstrcpy(&file, fsinfo->repos[repo], "/", stats->internal, 0) == -1)
		__retrieve_limit_finish(-1);
	sprintf(revision, "%dB", stats->rev);
	if (retrieve_rdiff(revision, file, node->tmp_path) != 0)
		__retrieve_limit_finish(-1);
	debug(3, "Retrieved to %s\n", node->tmp_path);
	if (stat(node->tmp_path, &temp) != 0 || temp.st_size != stats->size)
		__retrieve_limit_finish(-1);
	node->count = 1;

	// cache control
	lock(cache_mutex);
	if (node->count == 1)
		open_count++;
	unlock(file_mutex[repo][stats->rev]);
	free(file);
	free(revision);
	while ((open_count + cache_count > cache_limit) && (open_count < cache_limit))
		cache_delete(fsinfo);
	unlock(cache_mutex);

	return 0;

};

int __release_limit(struct stats *stats, int repo){

#define __release_limit_finish(value) {					\
			unlock(file_mutex[repo][stats->rev]);		\
			return value;								\
		}

    node_t *node = get_open_file(stats->path);
	lock(file_mutex[repo][stats->rev]);
	if (node->count > 1){
		node->count--;
		__release_limit_finish(0);
	}
	lock(cache_mutex);
	if (open_count > cache_limit){
		// nothing is cached; if open_count > cache_limit, then cache_count = 0
		open_count--;
		unlock(cache_mutex);
		node->count = 0;
		unlink(node->tmp_path);
        delete_open_file(node);
	}
	else {
		open_count--;
		cache_add(node);
		unlock(cache_mutex);
	};
	__release_limit_finish(0);

};

int cache_add(node_t *node){

	struct cache *temp = single(struct cache);

	debug(3, "Expanding cache with %s file with %d open and %d cached files;\n", node->path, open_count, cache_count);
	cache_count++;
	temp->node = node;
	if (cache == NULL){
		cache = temp;
		last = temp;
	}
	else{
		last->next = temp;
		last = temp;
	};
	return 0;
	
};

int cache_delete(struct file_system_info *fsinfo){

	struct cache *temp = NULL;
	int repo = 0;
	if (cache == NULL)
		return -1;
     if ((repo = repo_number(fsinfo, cache->node->path)) == -1)
        return -1;
	debug(3, "Deleting from cache %s file with %d open and %d cached files;\n", cache->node->path, open_count, cache_count);
	lock(file_mutex[repo][cache->node->rev]);
	if (cache->node->count > 1){
		cache->node->count--;
		open_count++;
	}
	else{
		cache->node->count = 0;
		unlink(cache->node->tmp_path);
		gstrdel(cache->node->tmp_path);
	};
	unlock(file_mutex[repo][cache->node->rev]);
	cache_count--;
	temp = cache;
	cache = cache->next;
	free(temp);
	return 0;

};
