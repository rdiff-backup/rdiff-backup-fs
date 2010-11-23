#ifndef _GPATH_H_
#define _GPATH_H_

// #define GPATH_DEBUG

#include "../headers.h"
#include "gstring.h"

/*
 * for given path creates new one without multiple '/' and '/' at the end and
 * on the begining of the string
 *
 * @1: location where original string is stored;
 * @2: location where modified string should be copied;
 * @3: length of the original string;
 * @4: length of the modified string;
 */
#define gpthcln(source, dest, len, new_len) {					\
	    dest = calloc(len + 1, sizeof(char));					\
	    int i = 0;												\
	    int j = 0;												\
	    while ((i < len) && (source[i] == '/'))				\
			i++;												\
	    for (; i < len; i++)									\
			if ((source[i] != '/') || (source[i + 1] != '/')){	\
		    	dest[j] = source[i];							\
		    	j = j + 1;										\
	        };													\
	    if (dest[j - 1] == '/')								\
			dest[j - 1] = 0;									\
	    new_len = strlen(dest);									\
	}

/*
 * for given path finds the most nested entry and copies it to the given location;
 *
 * @1: pointer to the location, where entry should be copied;
 * @2: path which is scrutinized;
 *
 * returns: 0 on success and -1 otherwise;
 */
int gpthcld(char **, char *);

/* 
 * for given path finds the most nested entry and sets another point at this place;
 *
 * @1: pointer to a string pointer, that will point at the beginning of the entry;
 * @2: path which is scrutinized;
 *
 * returns: 0 on success and -1 otherwise;
 */
int gpthcldptr(char **, char *);

/*
 * for given path of an archive turns it into path without the archive extensions; it doesn't unpack the file;
 *
 * @1: pointer to the string holding the path;
 *
 * returns: 0 on success and -1 otherwise;
 */
int gpthugz(char **);

/*
 * for given two paths it determines if one is direct child of another
 *
 * @1: path that is supposed to be a parent of the another one;
 * @2: path that is supposed to be a child of the another one;
 *
 * returns: 0 on success and -1 otherwise;
 */
int gpthent(const char *, const char *);

int gpthsub(const char *, const char *);

/*
 * given path is divided into parts (single steps) and stored in an 
 * array of strings
 *
 * @1: path that is supposed to be divided;
 * @2: location where array with parts will be stored;
 *
 * returns: size of the created array or -1 on failure;
 */
int gpthdiv(const char *, char ***);

/*
 * for a given path function extracts its certain part (single step)
 *
 * @1: path from which we extract certain part
 * @2: index of the part in the path, that we want to extract
 *
 * returns: part of the path or NULL on failure
 */
char* gpthprt(const char *, int);

/*
 * if path is not in absolute form, then function turns it into one; then function checks, whether 
 * file pointed by the path exists;
 *
 * @1: pointer to a path
 *
 * returns: 0 if file exists and function is successful, -1 otherwise
 */
int gpthpro(char **);

/*
 * for a given path function extracts file's extension
 *
 * @1: path to the file, which extension is extracted
 *
 * returns: extension of the file; memory must be freed
 */
char* gpthext(const char *);

/*
 * for a given path function returns it's depth, meaning number of
 * nested directories/files
 * 
 * @1: pointer to a path
 * 
 * returns: on success depth of the path, -1 otherwise
 */ 
int gpthdpt(const char *);

/*
 * function returns path created from the given by cutting top element
 * 
 * @1: pointer to a path
 * 
 * returns: a new path on success, NULL otherwise (this includes
 * situation, where returning path would be empty)
 */ 
char* gpthcut(const char *);

#endif
