#ifndef _SUPPORT_GRDIFF_H_
#define _SUPPORT_GRDIFF_H_

#include "gutils.h"
#include "gstats.h"
#include "gstring.h"
#include "gpath.h"
#include "gtree.h"
#include "headers.h"

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

int get_revisions(struct file_system_info *, char *, int);

/*
 * adds given revision to target file in a given directory
 * 
 * @1: revision file name to be added
 * @2: target file name
 * @3: directory where both files are stored
 */
int add_snapshot(char *, char *, char *);

/*
 * copy given revision to target file in a given directory
 * 
 * @1: revision file name to be copied
 * @2: target file name
 * @3: directory where both files are stored
 * 
 * returns: 0 on sucess, -1 otherwise
 */
int snapshot_copy(char *, char *, char *);

/*
 * append given revision to target file in a given directory
 * 
 * @1: revision file name to be appended
 * @2: target file name
 * @3: directory where both files are stored
 * 
 * returns: 0 on sucess, -1 otherwise
 */
int snapshot_append(char *, char *, char *);

int update_tree(tree_t, stats_t *, char *);

/*
 * finds and unzips revision from the given repository
 * 
 * @1: path to the rdiff-backup repository
 * @2: path to directory where revisions should be unzipped
 * @3: pointer which will point to allocated memory with revision names
 * 
 * returns: number of revisions found on success, -1 otherwise
 */ 
int gather_revisions(struct file_system_info *, char *, char *);

/*
 * adds repository stat to a structure tree
 * 
 * @1: repository name
 * @2: tree with structure
 * 
 * returns: returns 0 on sucess, -1 otherwise
 */
int add_repo_dir(char *, tree_t);

#endif
