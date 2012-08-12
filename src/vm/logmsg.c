#include <stdio.h>
#include <stdarg.h>
#include "logmsg.h"
#include "umach.h"

void logmsg(enum log_level level, const char* format, ...)
{
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
