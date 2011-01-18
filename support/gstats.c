#include "gstats.h"
#include "gstring.h"
#include "gutils.h"

void copy_stats(stats_t *source, stats_t **dest){
    debug(3, "copying stats for: path %s, internal: %s, name: %s, rev: %d\n",
          source->path, source->internal, source->name, source->rev);
    *dest = single(stats_t);
    memcpy((*dest), source, sizeof(stats_t));
    if (source->path){
        (*dest)->path = 0;
        gstrcpy(&(*dest)->path, source->path);
    }
    if (source->internal)
        (*dest)->internal = (*dest)->path + (source->internal - source->path);
    if (source->name)
        (*dest)->name = (*dest)->path + (source->name - source->path);
    if (source->link) {
        (*dest)->link = 0;
        gstrcpy(&(*dest)->link, source->link);
    }
    debug(4, "done copying stats: path %s, internal %s, name %s, rev: %d\n",
          (*dest)->path, (*dest)->internal, (*dest)->name, (*dest)->rev);
};
