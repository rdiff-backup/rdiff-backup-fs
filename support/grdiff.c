#include "grdiff.h"

// prototypes:

/*
 * unzip target file to target directory
 * 
 * @1: file to be unzipped
 * @2: directory where unzipped file should be stored
 * 
 * returns: 0 on sucesss, -1 otherwise
 */
int unzip(char *, char *);

// public:

int update_tree(tree_t tree, stats_t *stats){
    if (stats->type == -1)
        return gtreedel(tree, stats->path);
    else
        return gtreeadd(tree, stats);
}

int unzip_revs(char *path, char *dest){

    DIR *dir = NULL;
    int rev_count = 0;
    struct dirent *entry = NULL;
    char *mirror = NULL;
    char *extension = NULL;
    int descriptor = 0;

	// printf("[Function: unzip_revs] Unzipping revisions in %s directory;\n", path);
    if ((dir = opendir(path)) == NULL)
    	return -1;
    for (entry = readdir(dir); entry != NULL; entry = readdir(dir)){
        if (gstrsub(entry->d_name, "mirror_metadata.") == 0){
            if (gmstrcpy(&mirror, path, "/", entry->d_name, 0) == -1)
            	continue;
            extension = gpthext(entry->d_name);
            if (strcmp(extension, "gz") == 0){
	            gstrdel(extension);
            	if (unzip(mirror, dest) == -1)
            		continue;
            }
            else{
                gstrdel(extension);
            	if (gmstrcpy(&mirror, dest, "/", entry->d_name, 0) == -1)
            		continue;
            	if ((descriptor = open(mirror, O_WRONLY | O_CREAT)) == -1)
            		continue;
            	if (close(descriptor) == -1)
            		continue;
            };
            rev_count++;
        };
    };
    closedir(dir);
    return rev_count;
    
};

int count_revs(char *path){
    DIR *dir = NULL;
    int rev_count = 0;
    struct dirent *entry = NULL;
    
#ifdef DEBUG_DEEP
	printf("[Function: count_revs] Counting revisions in %s directory;\n", path);
#endif
    if ((dir = opendir(path)) == NULL)
    	return -1;
    for (entry = readdir(dir); entry != NULL; entry = readdir(dir))
        if (gstrsub(entry->d_name, "mirror_metadata.") == 0)
            rev_count++;
    closedir(dir);
#ifdef DEBUG_DEEP
	printf("[Function: count_revs] Found %d revisions;\n", rev_count);
#endif    
    return rev_count;
};

int gather_revisions(char *repo_path, char ***revisions) {
    
    #define gather_revisions_finish(value) {            \
        gstrdel(path);                                  \
        return value;                                   \
    }
    
    char *path = NULL;
    int count = 0;

	if (gmstrcpy(&path, repo_path, "/rdiff-backup-data", 0) != 0)
		gather_revisions_finish(-1);
    if ((count = unzip_revs(path, data_dir)) == -1)
    	gather_revisions_finish(-1);
    if (((*revisions) = calloc(count, sizeof(char *))) == NULL)
    	gather_revisions_finish(-1);
    if (get_revisions(data_dir, count, *revisions) == -1)
    	gather_revisions_finish(-1);
    gstrsort(*revisions, count);        
    gather_revisions_finish(count);
    
};

// FIXME: what about seasonal time?
time_t get_revs_date(char *mirror){

    #define FACTORS_COUNT 8
    #define FACTORS_SHORT_COUNT 6
    
    int i = 0;
    int factors[FACTORS_COUNT];
    char *current = mirror + strlen(MIRROR_PREFIX);
    char *next = 0;
    struct tm rev_time;
    
    if (mirror[GMT_TYPE_POSITION] == 'Z')
		for (i = 0; i < FACTORS_SHORT_COUNT; i++){
		    factors[i] = strtol(current, &next, 10);
		    current = next + 1;
		}
    else
		for (i = 0; i < FACTORS_COUNT; i++){
		    factors[i] = strtol(current, &next, 10);
		    current = next + 1;
		};

    rev_time.tm_year = factors[0] - 1900;
    rev_time.tm_mon = factors[1] - 1;
    rev_time.tm_mday = factors[2];
    
    rev_time.tm_hour = factors[3];
    rev_time.tm_min = factors[4];
    rev_time.tm_sec = factors[5];
        
    /* TODO: consult, whether seasonal time change should be considered */
    rev_time.tm_isdst = -1;
    
    return mktime(&rev_time);

};

char* get_revs_dir(char *mirror){

    time_t rev_date = get_revs_date(mirror);
    struct tm *rev_tm = gmtime(&rev_date);
    char *result = gstralloc(strlen(ARCHFS_DIR_FORMAT_LENGTH));
    
    sprintf(result, ARCHFS_DIR_FORMAT, rev_tm->tm_year + 1900, 
            rev_tm->tm_mon + 1, rev_tm->tm_mday, rev_tm->tm_hour, 
            rev_tm->tm_min, rev_tm->tm_sec);
    return result;
    
};

int read_stats(stats_t *stats, FILE *file){

	char *line = NULL;
	size_t length = 0;
	int result = 0;
	
	int name_set = 0;
	int link_set = 0;
	int type_set = 0;
	int size_set = 0;
	int time_set = 0;
	
	memset(stats, 0, sizeof(stats_t));
	while ((result = gstrline(&line, &length, file)) != -1){
		if (gstrsub(line, "File ") == 0){
			memset(stats, 0, sizeof(struct stats));
			line[result - 1] = 0;
			if (strcmp(line, "File .") == 0)
				continue;
			// gstrcpy(stats->internal, line + strlen("File "));
			gstrcpy(&stats->internal, line + 5);
			name_set = 1;
			link_set = 0;
			type_set = 0;
			size_set = 0;
			time_set = 0;
		}
		if (gstrsub(line, "  Size") == 0){
			// stats->size = atoi(line + strlen("  Size ");
			stats->size = atoi(line + 7);
			size_set = 1;
		};
		if (strcmp(line, "  Type reg\n") == 0){
			stats->type = S_IFREG;
			type_set = 1;
		};
		if (strcmp(line, "  Type dir\n") == 0){
			stats->type = S_IFDIR;
			type_set = 1;
		};
		if (strcmp(line, "  Type sym\n") == 0){
			stats->type = S_IFLNK;
			type_set = 1;
		};
		if (strcmp(line, "  Type None\n") == 0){
			stats->type = -1;
			if (name_set == 1)
				return 0;
		};
		if (gstrsub(line, "  SymData ") == 0){
			line[result - 1] = 0;
			// gstrcpy(stats->link, line + strlen("  SymData "));
			gstrcpy(&stats->link, line + 10);
			link_set = 1;
		};
		if (gstrsub(line, "  ModTime ") == 0){
			// stats->ctime = atoi(line + strlen("  ModTime "));
			stats->ctime = atoi(line + 10);
			stats->atime = stats->ctime;
			time_set = 1;
		}
		if ((stats->type == S_IFLNK) &&
			(name_set == 1) && (link_set == 1) && (type_set == 1))
			return 0;
		if ((stats->type == S_IFDIR) &&
			(name_set == 1) && (type_set == 1) && (time_set == 1))
			return 0;
		if ((stats->type != S_IFLNK) &&
			(name_set == 1) && (size_set == 1) && (type_set == 1) && (time_set == 1))
			return 0;
	};
#ifdef DEBUG_DEEP
	printf("[Function: read_stats] Finished reading file %s with name %s, type %s, size %s, time %s, link %s;\n",
		   stats->internal, name_set == 1 ? "set" : "not set", type_set == 1 ? "set" : "not set",
		   size_set == 1 ? "set" : "not set", time_set == 1 ? "set" : "not set", link_set == 1 ? "set" : "not set");
#endif
	return -1;

};

int get_revisions(char *where, int count, char **revs){
    
    DIR *dir = NULL;
    struct dirent *entry;
    int i = 0;

    // printf("[Function: get_revisions] Received place for %d revisions;\n", count);
    if ((dir = opendir(where)) == NULL)
        return -1;
    for (entry = readdir(dir); (i < count) && (entry != NULL); entry = readdir(dir))
        if (gstrsub(entry->d_name, "mirror_metadata.") == 0){
            gstrcpy(&revs[i], entry->d_name);
            i++;
        };
    closedir(dir);
    if (i != count)
        return -1;
    // printf("[Function: get_revisions] Retrieved and sorted %d revisions;\n", count);
    return 0;
    
}

int snapshot_copy(char *revision){
	
	char *path = NULL;
	char *snapshot = NULL;
	int snapshot_desc = 0;
	int revision_desc = 0;
	char buffer[1024];
	size_t result = 0;

	gmstrcpy(&path, data_dir, "/", revision, 0);
	gmstrcpy(&snapshot, data_dir, "/", CURRENT_SNAPSHOT, 0);
#ifdef DEBUG_DEEP
	printf("[Function: snapshot_copy] Copying from %s to %s\n", path, snapshot);
#endif
	unlink(snapshot);
	if ((snapshot_desc = open(snapshot, O_WRONLY | O_CREAT, S_IRWXU)) == -1)
		return -1;
	if ((revision_desc = open(path, O_RDONLY)) == -1){
		close(snapshot_desc);
		return -1;
	};
	while ((result = read(revision_desc, buffer, 1024)) != 0)
		write(snapshot_desc, buffer, result);
	close(snapshot_desc);
	close(revision_desc);
	return 0;
	
};

int snapshot_append(char *file){

	char *snapshot = NULL;
	int snapshot_desc = 0;
	char *revision = NULL;
	int revision_desc = 0;
	char buffer[1024];
	size_t result = 0;

	gmstrcpy(&revision, data_dir, "/", file, 0);
	gmstrcpy(&snapshot, data_dir, "/", CURRENT_SNAPSHOT, 0);
#ifdef DEBUG_DEEP
	printf("[Function: snapshot_append] Appending from %s to %s\n", revision, snapshot);
#endif
	if ((snapshot_desc = open(snapshot, O_WRONLY | O_APPEND)) == -1)
		return -1;
	if ((revision_desc = open(revision, O_RDONLY)) == -1){
		close(snapshot_desc);
		return -1;
	};
	buffer[0] = '\n';
	write(snapshot_desc, buffer, 1);
	while ((result = read(revision_desc, buffer, 1024)) != 0)
		write(snapshot_desc, buffer, result);
	close(snapshot_desc);
	close(revision_desc);
	return 0;
	
};

// private:

int unzip(char *path, char *dest){
    
#define unzip_error	{							\
    	    gstrdel(temp); 						\
    	    gstrdel(target); 					\
    	    gstrdel(buffer);					\
    	    if (file != NULL)					\
    			fclose(file);					\
    	    if (archive != NULL)				\
    			gzclose(archive);				\
    	    return -1; 						\
    	}
    
    char *temp = NULL;    
    char *target = NULL;
    char *buffer = NULL;
    int buffer_length = 1000;

    FILE *file = NULL;
    gzFile archive = NULL;

    if (gpthcld(&temp, path) == -1)
		unzip_error;
    if (gpthugz(&temp) == -1)
		unzip_error;
    if (gmstrcpy(&target, dest, "/", temp, 0) == -1)
		unzip_error;
	    
    if ((file = fopen(target, "w")) == NULL)
		unzip_error;
    if ((archive = gzopen(path, "rb")) == NULL)
		unzip_error;
    if ((buffer = gstralloc(buffer_length)) == NULL)
		unzip_error;	
    while (gzgets(archive, buffer, buffer_length) != Z_NULL)
		fprintf(file, "%s", buffer);
        
    fclose(file);
    gzclose(archive);
    gstrdel(temp);
    gstrdel(target);
        
    return 0;
        
};
