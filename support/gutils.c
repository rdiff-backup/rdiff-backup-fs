#include "gutils.h"

int debug_level = -1;

int gdesccopy(int source, int dest){
	char buffer[1024];
    int result = 0;
	while ((result = read(source, buffer, 1024)) > 0){
		if (write(dest, buffer, result) == -1)
            return -1;
    }
    if (result == -1)
        return -1;
    return 0;
};
