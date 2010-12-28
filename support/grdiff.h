#ifndef _SUPPORT_GRDIFF_H_
#define _SUPPORT_GRDIFF_H_

#include "gutils.h"
#include "gstats.h"
#include "gstring.h"
#include "gpath.h"
#include "gtree.h"
#include "../headers.h"
#include "../externs.h"

/*
 * unzips all rdiff-backup mirror_metadata files in a given directory
 * 
 * @1: path to the directory with mirror_metada
 * @2: path to where unzipped files should be copied
 * 
 * returns: 0 on success, -1 otherwise
 */
int unzip_revs(char *, char *);

/*
 * counts all rdiff-backup mirror_metada files in a given directory
 * 
 * @1: path to the directory
 * 
 * returns: number of revisions on success, -1 otherwise
 */
int count_revs(char *);

/*
 * returns gmt time of revision creation
 * 
 * @1: filename of revision's mirror_metadata file
 * 
 * returns: gmt time on success, -1 otherwise
 */

time_t get_revs_date(char *);

/*
 * returns name of directory for a revision
 * 
 * @1: filename of revision's mirror_metadata file
 * 
 * returns: name of directory on success, NULL otherwise
 */
char* get_revs_dir(char *);

/*
 * read single portion of file stats from a stream
 * 
 * @1: location of stats structure, where information should be set
 * @2: file stream
 * 
 * returns: 0 on success, -1 otherwise
 */
int read_stats(struct stats *stats, FILE *file);

int get_revisions(int, char **);

int snapshot_copy(char *);

int snapshot_append(char *file);

int update_tree(tree_t, stats_t *);

int gather_revisions(char *, char ***);

#endif
