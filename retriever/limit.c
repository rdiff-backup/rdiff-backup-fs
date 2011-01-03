#include "limit.h"

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

int __retrieve_limit(struct stats *, int);
int __release_limit(struct stats *, int);

int cache_add(struct stats *);
int cache_delete();

// public:

int retrieve_limit(struct file_system_info *fsinfo, struct stats *stats){

	int repo = 0;

#ifdef DEBUG
	printf("[Function: retrieve_limit] Retrieving file; currently %d open and %d cached files;\n", 
		   open_count, cache_count);
#endif
	if ((repo = repo_number(fsinfo, stats)) == -1)
		return -1;
	return __retrieve_limit(stats, repo);

};

int release_limit(struct file_system_info *fsinfo, struct stats *stats){

	int repo = 0;

#ifdef DEBUG
	printf("[Function: retrieve_limit] Releasing file; currently %d open and %d cached files;\n", 
		   open_count, cache_count);
#endif
	if ((repo == repo_number(fsinfo, stats)) == -1)
		return -1;
	return __release_limit(stats, repo);
		
};

// private:

int __retrieve_limit(struct stats *stats, int repo){

#define __retrieve_limit_finish(value){						\
			unlock(file_mutex[repo][stats->rev]);			\
			gstrdel(file);									\
			gstrdel(revision);								\
			return value;									\
		}

	char *file = NULL;
	char *revision = calloc(20, sizeof(char));
	struct stat *temp = single(struct stat);

	lock(file_mutex[repo][stats->rev]);
	// checking, whether file was already retrieved
	if (stats->shared > 0){
		stats->shared++;
		__retrieve_limit_finish(0);
	};

	// retrieving file
	if (create_tmp_file(stats) == -1)
		__retrieve_limit_finish(-1);
	if (gmstrcpy(&file, repos[repo], "/", stats->internal, 0) == -1)
		__retrieve_limit_finish(-1);
	sprintf(revision, "%dB", stats->rev);
	if (retrieve_rdiff(revision, file, stats->tmp_path) != 0)
		__retrieve_limit_finish(-1);
	printf("[Fuse: __retrieve_limit] Retrieved to %s\n", stats->tmp_path);
	if (stat(stats->tmp_path, temp) != 0)
		__retrieve_limit_finish(-1);
	stats->shared = 1;

	// cache control
	lock(cache_mutex);
	if (stats->shared == 1)
		open_count++;
	unlock(file_mutex[repo][stats->rev]);
	free(file);
	free(revision);
	while ((open_count + cache_count > cache_limit) && (open_count < cache_limit))
		cache_delete();
	unlock(cache_mutex);

	return 0;

};

int __release_limit(struct stats *stats, int repo){

#define __release_limit_finish(value) {					\
			unlock(file_mutex[repo][stats->rev]);		\
			return value;								\
		}

	lock(file_mutex[repo][stats->rev]);
	if (stats->shared > 1){
		stats->shared --;
		__release_limit_finish(0);
	}
	lock(cache_mutex);
	if (open_count > cache_limit){
		// nothing is cached; if open_count > cache_limit, then cache_count = 0
		open_count--;
		unlock(cache_mutex);
		stats->shared = 0;
		unlink(stats->tmp_path);
		gstrdel(stats->tmp_path);
	}
	else {
		open_count--;
		cache_add(stats);
		unlock(cache_mutex);
	};
	__release_limit_finish(0);

};

int cache_add(struct stats *stats){

	struct cache *temp = single(struct cache);

#ifdef DEBUG
	printf("[Function: cache add] Expanding cache with %s file with %d open and %d cached files;\n", stats->path, open_count, cache_count);
#endif
	cache_count++;
	temp->stats = stats;
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
	int repo = repo_number(fsinfo, cache->stats);
	
	if (cache == NULL)
		return -1;
#ifdef DEBUG
	printf("[Function: cache_delete] Deleting from cache %s file with %d open and %d cached files;\n", cache->stats->path, open_count, cache_count);
#endif
	lock(file_mutex[repo][cache->stats->rev]);
	if (cache->stats->shared > 1){
		cache->stats->shared--;
		open_count++;
	}
	else{
		cache->stats->shared = 0;
		unlink(cache->stats->tmp_path);
		gstrdel(cache->stats->tmp_path);
	};
	unlock(file_mutex[repo][cache->stats->rev]);
	cache_count--;
	temp = cache;
	cache = cache->next;
	free(temp);
	return 0;

};
