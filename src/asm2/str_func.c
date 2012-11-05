#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int str_to_int(const char *str, long *num) {
    if (str == NULL || num == NULL)
    	return FALSE;

    char *endprt = 0;
    
    /* we use the unsigned version strtoul
     * and not the signed strtol because
     * it works with signed values like
     * 0xAABBCCDD
     */
    long result = strtoul(str, &endprt, 0); // base "0" means base 16
    
    if (*endprt == '\0') {
    	*num = result;
        return TRUE;
    }
	
	return FALSE;
}

char *str_trim(char *str) {
    return g_strstrip(str);
}

void str_strip_comment(char *str) {
	while (*str) {
		if (*str == '#') {
			*str = '\0';
			return;
		}
		str++;
	}
}

void str_strip_comment_2(char *str) {
	char *c = strchr(str, '#');
	
	if (c != NULL)
		*c = '\0';
}

/*
char *get_nth_str(char *parts[], uint32_t n) {
    uint32_t i = 0, c = 0;

    while (parts[i] != NULL) {
        if (parts[i][0] == '\0') {
            i++;
            continue;
        }
        if (c == n)
            return parts[i];
        c++;
        i++;
    }

    return NULL;
}
*/
