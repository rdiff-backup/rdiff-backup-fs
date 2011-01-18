#ifndef _RETRIEVER_SUPPORT_H_
#define _RETRIEVER_SUPPORT_H_

#include <pthread.h>
#include "../headers.h"
#include "../externs.h"
#include "../errors.h"
#include "../support/gstring.h"
#include "../support/gpath.h"
#include "../structure/core.h"

#define lock(mutex) pthread_mutex_lock(&mutex)
#define unlock(mutex) pthread_mutex_unlock(&mutex)

struct node;

typedef struct node node_t;

struct node {
    char *path;
    char *tmp_path;
    int count;
    
    node_t *next;
    node_t *prev;
};

struct list {
    node_t *head;
    node_t *tail;
};

typedef struct list list_t;

extern pthread_mutex_t **file_mutex;

extern list_t *open_files;

node_t * get_open_file(char *path);

void delete_open_file(node_t *node);

int retriever_init_common(struct file_system_info *);

int retrieve_common(struct file_system_info *fsinfo, struct stats *, int);

int repo_number(struct file_system_info *, struct stats *);

int retrieve_rdiff(char *, char *, char *);

int create_tmp_file(struct stats *, node_t *node);

#endif
