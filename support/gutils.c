#include "gutils.h"

int gdesccopy(int source, int dest){
	char buffer[1024];
    int result = 0;
	while ((result = read(source, buffer, 1024)) > 0){
		write(dest, buffer, result);
    }
    if (result == -1)
        return -1;
    return 0;
};
