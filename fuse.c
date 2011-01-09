#include "fuse.h"

// public:

int revs_getattr(const char *path, struct stat *stbuf){

    struct stats *stats;
    memset(stbuf, 0, sizeof(struct stat));

	// printf("[FUSE: getattr] Attributes for path %s;\n", path);
    if (get_file(file_system_info, path, &stats) != 0) {
        // printf("[FUSE: getattr] Failed to retrieve stats;\n");
        return -1;
    }
    stbuf->st_size = stats->size;
    if (stats->type == S_IFDIR)
        stbuf->st_mode = stats->type | 0555;
    else
        stbuf->st_mode = stats->type | 0444;
    stbuf->st_nlink = stats->nlink;
    stbuf->st_mtime = stats->ctime;
    stbuf->st_ctime = stats->ctime;
    stbuf->st_atime = stats->atime;
	
    // printf("[FUSE: getattr] Retrieved attributes\n");
    return 0;
    
};

int revs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){

    (void) offset;
    (void) fi;

    int i = 0;
    struct stats *stats = 0;

#ifdef DEBUG
    printf("[FUSE: readdir] Received path %s;\n", path);
#endif
    if (get_file(file_system_info, path, &stats) != 0)
		return -ENOENT;
	
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    
    char **content = get_children(file_system_info, path);
    if (content == NULL){
#ifdef DEBUG
    	printf("[FUSE: readdir] ALERT: Error occured while looking for children;");
#endif
		return 0;
	}
    for (i = 0; content[i] != 0; i++)
		filler(buf, content[i], NULL, 0);
    if (stats != 0)
		stats->atime = time(0);
#ifdef DEBUG
	printf("[FUSE: readdir] There were %d children in this directory;\n", i);
#endif
    for (i = 0; content[i] != 0; i++)
        free(content[i]);
    free(content);
    return 0;
    
};

int revs_readlink(const char *path, char *buf, size_t size){

    (void) size;
	
	struct stats *stats;
	
	if ((get_file(file_system_info, path, &stats) != 0) || (stats->type != S_IFLNK))
		return -1;
	strcpy(buf, stats->link);		
	return 0;
	
};

int revs_open(const char *path, struct fuse_file_info *fi){

    (void) fi;

    struct stats *stats;
    
    get_file(file_system_info, path, &stats);
    if (stats->type & S_IFDIR)
		return -1;
	if (retrieve(file_system_info, stats) != 0)
		return -1;	    
    stats->atime = time(0);
    return 0;

};

int revs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){

    (void) fi;

#define revs_read_finish(value) {					\
			close(descriptor);						\
			return value;							\
		}

    struct stats *stats = NULL;
    int descriptor = 0;
    int result = 0;

    printf("[FUSE: Read] Reading file %s;\n", path);
    get_file(file_system_info, path, &stats);
    if ((stats == NULL) || (stats->shared == 0))
		return -1;
    if ((descriptor = open(stats->tmp_path, O_RDONLY)) == -1)
		return -1;
	if ((result = lseek(descriptor, offset, SEEK_SET)) != offset)
		revs_read_finish(-1);
	if ((result = read(descriptor, buf, size)) == -1)
		revs_read_finish(-1);
    printf("[FUSE: Read] %d bytes have been read;\n", result);
	revs_read_finish(result);
	
}

int revs_release(const char *path, struct fuse_file_info *fi){

    (void) fi;

    struct stats *stats = NULL;
    
    get_file(file_system_info, path, &stats);
    if (stats == NULL)
		return -1;
    return release(file_system_info, stats);

};

void revs_destroy(void *ptr){

    (void) ptr;

	DIR *dir = NULL;
	char *path = NULL;
	struct dirent *entry = NULL;

    if (data_dir == NULL)
    	return;
    dir = opendir(data_dir);
    if (dir == NULL)
    	return;
    for (entry = readdir(dir); entry != NULL; entry = readdir(dir)){
    	gmstrcpy(&path, data_dir, "/", entry->d_name, 0);
    	unlink(path);
    };
    closedir(dir);
#ifdef DEBUG
    printf("[FUSE: Destroy] Deleting temporary directory %s;\n", data_dir);
#endif
    rmdir(data_dir);
    gstrdel(path);

};
