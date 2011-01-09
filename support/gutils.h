#include <unistd.h>
#include <stdio.h>

/*
 * copies data from source descriptor to dest descriptor
 */
int gdesccopy(int source, int dest);

extern int debug_level;

/*
 * very simple macro for debug messages with multiple debug levels
 */
#define debug(level, format, ...) {                     \
    if (level <= debug_level)                           \
        printf("[%s, %d, %s] " format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);                  \
}
