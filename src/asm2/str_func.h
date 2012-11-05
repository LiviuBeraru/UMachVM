#ifndef STR_FUNC_H
#define STR_FUNC_H

int   str_to_int(const char *str, long *num);
char *str_trim(char *str);
void  str_strip_comment(char *str);
void  str_strip_comment_2(char *str);
char *get_nth_str(char *parts[], uint32_t n);

#endif
