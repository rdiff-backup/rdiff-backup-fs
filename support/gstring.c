#include "gstring.h"
#include "gutils.h"

// private:

// dest != NULL, (*dest) != NULL, sufix != NULL
int __gstrcat(char **dest, const char *sufix){

    int dest_length = strlen((*dest));
    int sufix_length = strlen(sufix);
    int i = 0;
    char temp[dest_length + sufix_length + 1];
    
    for (i = 0; i < dest_length; i++)
		temp[i] = (*dest)[i];
    for (i = 0; i < sufix_length; i++)
		temp[i + dest_length] = sufix[i];
    temp[dest_length + sufix_length] = 0;
    free((*dest));
    (*dest) = gstralloc(dest_length + sufix_length);
    if ((*dest) == NULL)
		return -1;
    gstrcpy(dest, temp);
    return 0;

};

int __gstrpart(char **array, int first, int last){
    char *medium = array[first];
    int i = first - 1;
    int j = last + 1;
    while (1) {
		do {
	    	j--;
		} while (strcmp(array[j], medium) > 0);
		do {
	    	i++;
		} while (strcmp(array[i], medium) < 0);
		if (i < j)
		    gstrswp(&array[i], &array[j]);
		else
	    	return j;
    };
};

void __gstrsort(char **array, int first, int last){
    if (first >= last)
		return;
    int middle = __gstrpart(array, first, last);
    __gstrsort(array, first, middle);
    __gstrsort(array, middle + 1, last);
};

int __gstrncpy(char **dest, const char *source, int count){
    
    if ((*dest) != NULL)
		free(*dest);
    if (((*dest) = gstralloc(count)) == NULL)
		return -1;
    strncpy((*dest), source, count);
    return 0;
    
};

// public:

int gstrcpy(char **dest, const char *source){

    if ((source == NULL) || (dest == NULL))
		return -1;
    if ((*dest) == source)
		return 0;	
    int length = strlen(source);
    return __gstrncpy(dest, source, length);
	    
};

int gstrncpy(char **dest, const char *source, int count){
    
    if ((source == NULL) || (dest == NULL))
		return -1;
    if ((*dest) == source)
		return 0;
    int length = strlen(source);
    if (length > count)
		return __gstrncpy(dest, source, count);
    else // length <= count
		return __gstrncpy(dest, source, length);
    
};


int gmstrcpy(char **dest, const char *source, ...){

    int length = 0;
    char *other_source = NULL;
    char *temp = NULL;
    va_list args;

    if (dest == NULL)
		return -1;
    if (source == NULL)
		return -1;
    length += strlen(source);
    va_start(args, source);
    while ((other_source = va_arg(args, char *)) != NULL)
		length += strlen(other_source);
    va_end(args);
    temp = gstralloc(length);
    strcat(temp, source);
    va_start(args, source);
    while ((other_source = va_arg(args, char *)) != NULL)
		strcat(temp, other_source);
    va_end(args);
    gstrcpy(dest, temp);
    gstrdel(temp);
    return 0;


};

int gstrsub(const char *main, const char *sub){
    
    int main_length = strlen(main);
    int sub_length = strlen(sub);
    int i = 0;

    if (sub_length > main_length)
		return -1;
    for (i = 0; i < sub_length; i++)
		if (main[i] > sub[i])
	    	return 1;
		else if (main[i] < sub[i])
	    	return -1;
    return 0;
    
};

// service *main == sufix
int gstrcat(char **main, const char *sufix){

    if (sufix == NULL)
		return -1;
    if (main == NULL)
		return -1;
    if ((*main) == NULL)
		return gstrcpy(main, sufix);
    return __gstrcat(main, sufix);
	    
};

int gmstrcat(char **main, const char *sufix, ...){

    int length = 0;
    char *other_sufix = NULL;
    char *temp = NULL;
    va_list args;
   
    if (main == NULL)
		return -1;
    if (sufix == NULL)
		return -1;
    
    if ((*main) != NULL)
		length = strlen((*main));
    length += strlen(sufix);
    va_start(args, sufix);
    while ((other_sufix = va_arg(args, char *)) != NULL)
		length += strlen(other_sufix);
    va_end(args);
    
    temp = gstralloc(length);
    if ((*main) != NULL)
		strcpy(temp, (*main));
    strcat(temp, sufix);
    va_start(args, sufix);
    while ((other_sufix = va_arg(args, char *)) != NULL)
		strcat(temp, other_sufix);
    va_end(args);
    
    gstrdel((*main));
    gstrcpy(main, temp);
	gstrdel(temp);
    return 0;
    
};

int gstrsort(char **array, int size){
    
    debug(4, "sorting array of size %d\n", size);
    __gstrsort(array, 0, size - 1);
    debug(4, "done sorting");
    return 0;
    
};

int gstrswp(char **first, char **second){

    char *temp = (*first);

    (*first) = (*second);
    (*second) = temp;
    
    return 0;

};

#ifndef HAVE_GETLINE

int gstrline(char **line, size_t *length, FILE *file){
	
	int filled = 0;
	char next = 0;
	char *temp = NULL;
	
	if (line == NULL)
		return -1;
	if ((*line == NULL) || (*length <= 0)){
		*line = gstralloc(128);
		*length = 129;
	}
	else
		memset(*line, 0, *length);

	while (1){
		next = (char) getc(file);
		if (next == EOF){
			if (filled == 0)
				return -1;
			else
				return filled;
		};
		if (filled == *length){
			gstrcpy(&temp, *line);
			gstrdel(*line);
			*length = (*length - 1) * 2 + 1;
			*line = gstralloc(*length);
			strcpy(*line, temp);
		};
		(*line)[filled] = next;
		filled++;
		if (next == '\n')
			return filled;
	};
	
	gstrdel(temp);
	return filled;
	
};

#endif
