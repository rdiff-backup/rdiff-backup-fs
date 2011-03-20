#include "fuse.h"
#include "headers.h"
#include "support/gutils.h"
#include "support/gstring.h"
#include "support/gstats.h"
#include "externs.h"
#include "errors.h"
#include "retriever/retriever.h"
#include "layout/core.h"

// public:

int revs_getattr(const char *path, struct stat *stbuf){

    struct stats *stats;
    memset(stbuf, 0, sizeof(struct stat));

	debug(1, "Attributes for path %s;\n", path);
    if (get_file(file_system_info, path, &stats) != 0) {
        debug(1, "Failed to retrieve stats;\n");
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
    stbuf->st_uid = stats->uid;
    stbuf->st_gid = stats->gid;
    
    gstrdel(stats->path);
	free(stats);
    debug(1, "Retrieved attributes\n");
    return 0;
    
};

int revs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){

    (void) offset;
    (void) fi;

    int i = 0;
    struct stats *stats = NULL;

    debug(1, "Received path %s;\n", path);
    if (get_file(file_system_info, path, &stats) != 0)
		return -ENOENT;
    gstrdel(stats->path);
    free(stats);
	
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    
    debug(1, "Reading content of the eturned directory\n");
    char **content = get_children(file_system_info, path);
    if (content == NULL){
    	debug(1, "Error occured while looking for children;");
		return 0;
	}
    for (i = 0; content[i] != 0; i++)
		filler(buf, content[i], NULL, 0);
	debug(1, "There were %d children in this directory;\n", i);
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
    
    if (get_file(file_system_info, path, &stats) == -1)
        return -1;
    if (stats->type & S_IFDIR)
		return -1;
	int result = retrieve(file_system_info, stats);
    gstrdel(stats->path);
    free(stats);
    return result;

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

    debug(1, "Reading file %s;\n", path);
    if (get_file(file_system_info, path, &stats) == -1)
        return -1;
    if (stats->type & S_IFDIR)
		return -1;
    char *tmp_path = get_tmp_path(stats->path);
    debug(1, "Retrieved stats for path %s with tmp path %s\n", stats->path, tmp_path);
    gstrdel(stats->path);
    free(stats);
    if (!tmp_path)
		return -1;
    if ((descriptor = open(tmp_path, O_RDONLY)) == -1)
		return -1;
	if ((result = lseek(descriptor, offset, SEEK_SET)) != offset)
		revs_read_finish(-1);
	if ((result = read(descriptor, buf, size)) == -1)
		revs_read_finish(-1);
    debug(1, "%d bytes have been read;\n", result);
	revs_read_finish(result);
	
}

int revs_release(const char *path, struct fuse_file_info *fi){

    (void) fi;

    struct stats *stats = NULL;
    
    get_file(file_system_info, path, &stats);
    if (stats == NULL){
        gstrdel(stats->path);
        free(stats);
        return -1;
    }
    int result = release(file_system_info, stats);
    gstrdel(stats->path);
    free(stats);    
    return result;

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
    debug(1, "Deleting temporary directory %s;\n", data_dir);
    rmdir(data_dir);
    gstrdel(path);

};
