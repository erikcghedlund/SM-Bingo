#ifndef LOG_H
#define LOG_H
#include <stdio.h>

enum log_level {NOT_SET, DEBUG, INFO, WARNING, ERROR, CRITICAL};


void set_log_out(FILE * f);

int log_not_set(const char * fmt, ...);

int log_debug(const char * fmt, ...);

int log_info(const char * fmt, ...);

int log_warning(const char * fmt, ...);

int log_error(const char * fmt, ...);

int log_critical(const char * fmt, ...);

#endif
