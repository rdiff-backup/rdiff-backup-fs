#ifndef _SUPPORT_GSTATS_H_
#define _SUPPORT_GSTATS_H_

#include "../headers.h"

/*
 * basic structure holding description of one file stored in the rdiff-backup 
 * archive;
 *
 * @path: full path of the file in the created filesystem;
 * @internal: full path of the file in the rdiff-backup repository; keep in 
 *            mind, that this field may point at a part 
 *			  of path field to keep memory usage lower; free memory accordingly
 * @name: name of the file, that will be displayed in the filesystem; keep in 
 *        mind, that this field may point at a part
 *        of path field to keep memory usage lower; free memory accordingly;
 * @rev: number of revision but in backward order, so revision 0 is written as
 *       rev_count - 1
 */
struct stats { 
 
    char *path;
    char *internal; 
    char *name;
    char *link;

    char *tmp_path; 
                  
    int type;
    int size;
    int shared; 
    nlink_t nlink; 
    time_t ctime; 
    time_t atime; 

    int rev;

};

typedef struct stats stats_t;

#define set_directory_stats(stats){            	    \
            (stats)->nlink = 1;                   	\
            (stats)->type = S_IFDIR;              	\
            (stats)->rev = -1;                    	\
            (stats)->internal = NULL;             	\
            (stats)->ctime = time(0);             	\
            (stats)->atime = time(0);             	\
            (stats)->size = DIR_SIZE;             	\
} 

void copy_stats(stats_t *source, stats_t *dest);

#endif
