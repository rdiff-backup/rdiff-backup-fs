#ifndef _GSTRING_
#define _GSTRING_

#include "../headers.h"

int gstrcpy(char **, const char *);

int gstrncpy(char **, const char *, int);

int gmstrcpy(char **, const char *, ...);

int gstrsub(const char *, const char *);

int gstrcat(char **, const char *);

int gmstrcat(char **, const char *, ...);

int gstrsort(char **, int);

/*
 * swaps two strings; first will be held under the location of the second and vice vers;
 *
 * @1: pointer to the location of the first string;
 * @2: pointer to the location of the second string;
 * 
 * returns: 0 on success, -1 otherwise;
 */
int gstrswp(char **, char **);

#define gstralloc(length) calloc(length + 1, sizeof(char))

#define gstrdel(string){				\
	    if (string != NULL){			\
			free(string);				\
			string = NULL;				\
	    }								\
	}
	
#endif

#ifdef HAVE_GETLINE
	#define gstrline(line, length, file) getline(line, length, file)
#else
	int gstrline(char **, size_t *, FILE *);
#endif
