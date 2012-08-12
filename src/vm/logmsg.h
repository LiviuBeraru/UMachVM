#ifndef LOG_H
#define LOG_H

enum log_level {
    LOG_ERR,
    LOG_WARN,
    LOG_INFO
};

void logmsg(enum log_level level, const char* format, ...);

#endif
