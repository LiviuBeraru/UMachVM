#include <stdio.h>
#include <stdarg.h>
#include "logmsg.h"
#include "options.h"

/**
 * This function behaves mostly like the standard printf function, except that
 * it takes an additional argument: the error or message level.
 * The levels are defined in logmsg.h. Depending on the verbosity value stored
 * in the program settings (structure options in umach.h), the message will be
 * printed or not.
 */
void logmsg(enum log_level level, const char* format, ...)
{
    /* if the verbosity level is too low, don't do anything. */
    if (level > options.verbose) {
        return;
    }
    
    switch (level) {
    case LOG_ERR:
        fprintf(stderr, "ERROR: ");
        break;
    case LOG_WARN:
        fprintf(stderr, "WARNING: ");
        break;
    case LOG_INFO:
        fprintf(stderr, "INFO: ");
        break;
    }
    
    va_list al;
    va_start (al, format);
    vfprintf(stderr, format, al);
    fprintf(stderr, "\n");
    va_end(al);
}
