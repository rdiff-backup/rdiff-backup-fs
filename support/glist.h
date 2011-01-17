#include "grdiff.h"

typedef struct list_node list_node_t;

struct list_node {
    stats_t *stats;
    list_node_t *next;
    list_node_t *prev;
};

typedef struct list list_t;

struct list {
    list_node_t *head;
    list_node_t *tail;
};

int list_new(list_t **);

int list_add(list_t *, stats_t *);

int list_find_by_path(list_t *, char *, stats_t **);
