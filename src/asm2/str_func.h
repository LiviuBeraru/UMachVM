#ifndef STR_FUNC_H
#define STR_FUNC_H

int   str_to_int(const char *str, long *num);
void  int_to_str(const int32_t value, char *string);
char *str_trim(char *str);
void  str_strip_comment(char *str);

#endif
