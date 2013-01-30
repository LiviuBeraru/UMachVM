/*
 * str_func.c
 * 
 * helper functions for string translation / manipulation
 * 
 */

#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int str_to_int(const char *str, long *num) {
    if (str == NULL || num == NULL)
        return FALSE;

    char *endprt = 0;
    
    /* we use the unsigned version strtoul()
     * and not the signed strtol() because
     * it works with signed values like
     * 0xAABBCCDD
     */
    long result = strtoul(str, &endprt, 0); // base 0 accepts DEC, 0xHEX, 0OCTAL
    
    if (*endprt == '\0') {
        *num = result;
        return TRUE;
    }
    
    return FALSE;
}

void int_to_str(const int32_t value, char *string) {
    char buf[12];
    size_t len;
    
    /* special case where abs() would fail */
    if (value == INT32_MIN) {
        strcpy(string, "−2147483648");
        return;
    }
    
    const char neg = value < 0 ? TRUE : FALSE;
    int32_t val = abs(value);

    for (int i = 0; TRUE; i++) {
        buf[i] = '0' + (char) (val % 10);
        val /= 10;

        if (val == 0) {
            if (neg) {
                buf[i + 1] = '-';
                buf[i + 2] = '\0';
                len = i + 2;
            } else {
                buf[i + 1] = '\0';
                len = i + 1;
            }
            
            break;
        }
    }

    string[len] = '\0';

    for (int i = 0; i < len; i++)
        string[len - i - 1] = buf[i];
}

char *str_trim(char *str) {
    return g_strstrip(str);
}

void str_strip_comment(char *str) {
    char *c = strchr(str, '#');
    
    if (c != NULL)
        *c = '\0';
}
