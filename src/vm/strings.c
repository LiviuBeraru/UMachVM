#include <string.h>
#include <stdlib.h> // strtol
#include <ctype.h>  // isspace

const char *byspace = " \t\n";

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

/** Return a pointer to the first non blank character
    in string or NULL if the string contains only whitespace
    characters.
 */
char *find_nonblank(char *string)
{
    while (*string != '\0') {
        if (! isspace(*string)) {
            return string;
        }
        string++;
    }
    return NULL;
}

int str_to_int(const char* number, long* n)
{
    if (number == NULL || n == NULL) return -1;
    long nn = 0;
    
    char *endprt = 0;
    nn = strtoul(number, &endprt, 0);
    if (*endprt) {
        return -1;
    } else {
        *n = nn;
        return 0;
    }
}
