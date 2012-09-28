#include <string.h>
#include <stdlib.h> // strtoul
#include <ctype.h>  // isspace

/** Split string into maximum max items using the specified split pattern.
    The items are stored into the array items, which is supposed to be
    at least max long. */
int split(char *string, const char *pattern, char *items[], size_t max)
{
    memset(items, 0, max * sizeof(*items));
    int i = 0;
    char *item = NULL;

    item = strtok(string, pattern);
    while(i < max && item != NULL) {
        items[i] = item;
        i++;
        item = strtok(NULL, pattern);
    }

    return i;
}

int str_to_int(const char* number, long* n)
{
    if (number == NULL || n == NULL) return -1;
    long nn = 0;

    char *endprt = 0;
    /* we use the unsigned version strtoul
     * and not the signed strtol because
     * it works with signed values like
     * 0xAABBCCDD
     */
    nn = strtoul(number, &endprt, 0);
    if (*endprt) {
        return -1;
    } else {
        *n = nn;
        return 0;
    }
}

char *str_trim(char* string)
{
    /* protect against our own stupidity */
    if (string == NULL || *string == '\0') {
        // nothing to trim
        return string;
    }
    
    /* skip leading whitespace*/
    while(isspace(*string)) {
        *string = '\0';
        string++;
    }
    
    int len = strlen(string);
    if (len < 1) {
        /* nothing to do */
        return string;
    }
    
    char *last = string + len - 1;
    while (isspace(*last)) {
        *last = '\0';
        last--;
    }
    
    return string;
}
