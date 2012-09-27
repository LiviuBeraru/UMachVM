#ifndef STRINGS_H
#define STRINGS_H

int split(char *string, const char *pattern, char *items[], size_t max);
int str_to_int(const char *number, long *n);
char *str_trim(char *string);

#endif 
