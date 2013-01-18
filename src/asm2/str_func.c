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
    char *c = strchr(str, '#');
    
    if (c != NULL)
        *c = '\0';
}

