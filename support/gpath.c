#include "gpath.h"
#include "gutils.h"
	    
// private:

int __gpthcld(char **dest, char *source){

    int source_length = strlen(source);
    int i = 0;

	debug(4, "Received path %s;\n", source);
    if (source[source_length - 1] != '/')
		for (i = source_length - 1; (i >= 0) && (source[i] != '/'); i--);
    else // source[source_length - 1] == '/'
		for (i = source_length - 2; (i >= 0) && (source[i] != '/'); i--);

    gstrdel((*dest));    
    if (((*dest) = gstralloc(source_length - i -1)) == NULL)
    	return -1;
    if (source[source_length - 1] != '/')
		strcpy((*dest), source + i + 1);
    else
		strncpy((*dest), source + i + 1, source_length - i - 2);
    return 0;
    
};
    
int __gpthcld_own(char **dest, char *source){

    int source_length = strlen(source);
    int i = 0;

	debug(4, "Received path %s;\n", source);
    if (source[source_length - 1] != '/')
		for (i = source_length - 1; (i >= 0) && (source[i] != '/'); i--);
    else // source[source_length - 1] == '/'
		for (i = source_length - 2; (i >= 0) && (source[i] != '/'); i--);

    // strlen(temp) = source_length - i - 1 + 1
    char temp[source_length - i];
    strcpy(temp, source + i + 1);
    temp[source_length - i - 1] = 0;

    gstrdel((*dest));
    if (((*dest) = gstralloc(source_length - i - 1)) == NULL)
    	return -1;
    if (temp[source_length - i - 2] != '/')
		strcpy((*dest), temp);
    else
		strncpy((*dest), temp, source_length - i - 2);

    return 0;

};

// public:

int gpthcld(char **dest, char *source){

    if (dest == NULL)
		return -1;
    if (source == NULL)
		return -1;
    if ((*dest) == source)
		return __gpthcld_own(dest, source);
    else
		return __gpthcld(dest, source);

};

int gpthcldptr(char **pointer, char *path){

    if (pointer == NULL)
		return -1;
    if (path == NULL)
		return -1;
	
    int i = strlen(path) - 1;
    for (; (i >= 0) && (path[i] == '/'); i--);
    if (i < 0)
		return -1;
    for (; (i >= 0) && (path[i] != '/'); i--);
    	(*pointer) = path + i + 1;
    return 0;

};

// public

int gpthugz(char **path){

#define gpthugz_finish(value) {	\
	    gstrdel(temp);			\
	    return value;				\
	}

    char *temp = NULL;

    if (path == NULL)
		gpthugz_finish(-1);
    if ((*path) == NULL)
		gpthugz_finish(-1);
    int length = strlen((*path));
    if (length < strlen("*.gz"))
		gpthugz_finish(-1);
	
    (*path)[length - 1] = 0;
    (*path)[length - 2] = 0;
    (*path)[length - 3] = 0;

    if (gstrcpy(&temp, (*path)) != 0)
		gpthugz_finish(-1);
    if (gstrcpy(path, temp) != 0)
		gpthugz_finish(-1);
	gpthugz_finish(0);

}

int gpthent(const char *path, const char *entry){

#define gpthent_finish(value) {						\
			gstrdel(__path);						\
			gstrdel(__entry);						\
			return value;							\
		}

    if ((path == NULL) || (entry == NULL))
		return -1;

    int path_length = strlen(path);
    int entry_length = strlen(entry);
    int i = 0;
    
    char *__path = NULL;
    char *__entry = NULL;
    int __path_length = 0;
    int __entry_length = 0;

    if ((path_length == 0) || (entry_length == 0))
		gpthent_finish(-1);
    gpthcln(path, __path, path_length, __path_length);
    gpthcln(entry, __entry, entry_length, __entry_length);

    if (gstrsub(__entry, __path) != 0)
		gpthent_finish(-1);
    if ((__entry_length == 0) || (__entry_length == __path_length))
		gpthent_finish(-1);
    // don't need to check size here
    if ((__entry[__path_length] != '/') && (__path_length != 0))
		gpthent_finish(-1);
	// i = __path_length + 1;
    for (i = __path_length + 1; (i < __entry_length); i++)
		if (__entry[i] == '/')
	    	gpthent_finish(-1);
	gpthent_finish(0);
    
};

int gpthsub(const char *path, const char *entry){
    
#define gpthsub_finish(value) {						\
			gstrdel(__path);						\
			gstrdel(__entry);						\
			return value;							\
		};
    
    if ((path == NULL) || (entry == NULL))
		return -1;
    int path_length = strlen(path);
    int entry_length = strlen(entry);
    if ((path_length == 0) || (entry_length == 0))
		return -1;
    char *__path = NULL;
    char *__entry = NULL;
    int __path_length = 0;
    int __entry_length = 0;
    
    gpthcln(path, __path, path_length, __path_length);
    gpthcln(entry, __entry, entry_length, __entry_length);
    
    if (gstrsub(__entry, __path) != 0)
		gpthsub_finish(-1);
    if ((__entry_length == 0) || (__entry_length == __path_length))
		gpthsub_finish(-1);
    if ((__entry[__path_length] != '/') && (__path_length != 0))
		gpthsub_finish(-1);
    gpthsub_finish(0);
    
};

int gpthdiv(const char *path, char ***parts){
    
#define gpthdiv_error {						\
			gstrdel(__path);				\
			if ((*parts) != NULL){			\
				free(*parts);				\
				(*parts) = NULL;			\
			}								\
			return -1;						\
		};
    
    if ((path == NULL) || (parts == NULL))
		return -1;
	if ((*parts) != NULL){
		free(*parts);
		(*parts) = NULL;
	};
    int path_size = strlen(path);
    char *__path = NULL;
    int __path_size = 0;
    int parts_count = 0;
    int i = 0, j = 0;
    int current = 0;
    
    gpthcln(path, __path, path_size, __path_size);
    if (__path_size == 0)
		gpthdiv_error;
    parts_count = 1;
    for (i = 0; i < __path_size; i++)
		if (__path[i] == '/')
	    	parts_count++;
    if (((*parts) = calloc(parts_count, sizeof(char *))) == NULL)
    	gpthdiv_error;
    
    j = 0;
    current = 0;
    for (i = 0; i < __path_size; i++)
		if (__path[i] == '/'){
	    	if (gstrncpy(&((*parts)[current]), __path + j, i - j) == -1)
	    		gpthdiv_error;
		    j = i + 1;
		    current = current + 1;
		}
    if (gstrncpy(&((*parts)[current]), __path + j, i - j) == -1)
    	gpthdiv_error;

	free(__path);
	return parts_count;
    
};

// TODO: optimize; don't use gpthdiv
char* gpthprt(const char *path, const int index){

	#define gpthprt_finish(result){				\
		for (i = 0; i < count; i++)				\
			free(parts[i]);						\
		free(parts);							\
		return result;							\
	}

	char **parts = NULL;
	int count = gpthdiv(path, &parts);
	char *result = NULL;
	int i = 0;
	
	// implies count > 0
	if ((index >= count) || (index < 0))
		gpthprt_finish(NULL);
	if (gstrcpy(&result, parts[index]) == -1)
		result = NULL;
	gpthprt_finish(result);
	
};

int gpthpro(char **path){

#define gpthpro_finish(value) {					\
			gstrdel(current);					\
			return value;						\
		};

	char *current = NULL;
	struct stat temp;
	
	if ((path == NULL) || (*path == NULL))
		return -1;
	if (*path[0] == '/'){
		if (stat(*path, &temp) == 0)
			return 0;
		else
			return -1;
	}
	else{ // path[0] != '/'
#ifdef _GNU_SOURCE
		gmstrcpy(&current, get_current_dir_name(), "/", *path, 0);
#else
		int length = 20;
		current = gstralloc(length);
		while (getcwd(current, length) == NULL){
			free(current);
			length *= 2;
			current = gstralloc(length);
		};
		gmstrcat(&current, "/", *path, 0);
		printf("%s\n", current);
#endif
		if (stat(current, &temp) == 0){
			gstrcpy(path, current);
			gpthpro_finish(0);
		}
		else
			gpthpro_finish(-1);
	};

};

char* gpthext(const char *path){
	
#define gpthext_return(value) {								\
			gstrcpy(&result, value);						\
			return result;									\
		}
    
    char *temp = gpthextptr((char *) path);
    char *result = NULL;
    
    if (!temp)
        return NULL;
    gstrcpy(&result, temp);
    return result;
	
};

char * gpthextptr(char *path){
    int i = 0;
    if (path == NULL)
        return NULL;
    for (i = strlen(path) - 1; i >= 0 && path[i] != '.'; i--);
    if (i <= 0)
        return path + strlen(path) - 1; // returning pointer to an empty string
    else
        return path + i + 1;
};

int gpthdpt(const char *path){

	char **parts = NULL;
	int depth = 0;
	int count = 0;
	int i = 0;
	
	if (path == NULL)
		return -1;
	count = gpthdiv(path, &parts);
	if ((count == -1) || (count == 0))
		return count;
	for (i = 0; i < count; i++)
		if (strcmp(parts[i], "..") == 0)
			depth--;
		else
			depth++;
	return depth;
	
};

char* gpthcut(const char *source){
	
	int length = 0, new_length = 0, i = 0;
	char *temp = NULL, *dest = NULL;
	
	if (source == NULL)
		return NULL;
	length = strlen(source);
	gpthcln(source, temp, length, new_length);
	for (i = 0; (i < new_length) && (temp[i] != '/'); i++);
	if (i == new_length){
		gstrdel(temp);
		return NULL;
	};
	gstrcpy(&dest, temp + i + 1);
	gstrdel(temp);
	return dest;
	
};
