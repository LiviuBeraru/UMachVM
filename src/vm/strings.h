#ifndef STRINGS_H
#define STRINGS_H

int   split(char *string, const char *pattern, char *items[], size_t max);
char *find_nonblank(char *string);
int   str_to_int(const char *number, long *n);

extern const char *byspace;

#endif 
