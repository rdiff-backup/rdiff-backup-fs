#include "glist.h"

int list_new(list_t **list) {
    if (((*list) = single(list_t)) == NULL)
        return -1;
    (*list)->head = (*list)->tail = NULL;
    return 0;
};

int list_add(list_t *list, stats_t *stats) {
    if (!list->head)
        list->head = list->tail = single(list_node_t);
    else {
        list_node_t *node = single(list_node_t);
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }
    list->tail->stats = stats;
    return 0;
};

int list_find_by_path(list_t *list, char *path, stats_t **stats){
    list_node_t *node;
    for (node = list->head; (node) && (strcmp(node->stats->path, path) != 0); node = node->next)
        if (!node->stats->shared){
            // deleting stats, that aren't open any more, since it makes no
            // real sense to cache them anymore
            if (node->next)
                node->next->prev = node->prev;
            if (node->prev)
                node->prev->next = node->next;
            free(node->stats);
            free(node);
        }
    if (node) {
        (*stats) = node->stats;
        return 0;
    }
    return -1;
};
