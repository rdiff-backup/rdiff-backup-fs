#include "gstats.h"
#include "gstring.h"

void copy_stats(stats_t *source, stats_t *dest){
    memcpy(dest, source, sizeof(stats_t));
    if (source->path){
        dest->path = 0;
        gstrcpy(&dest->path, source->path);
    }
    dest->internal = dest->path + (source->path - source->internal);
    dest->name = dest->path + (source->path - source->name);
    if (source->link) {
        dest->link = 0;
        gstrcpy(&dest->link, source->link);
    }
};
