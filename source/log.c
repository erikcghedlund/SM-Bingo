#include <time.h>
#include <stdio.h>
#include <stdarg.h>

#include "log.h"

#define BUFFERSIZE 32

FILE * log_out = NULL;
enum log_level cur_level = NOT_SET;

char * timestr(char * buf) {
    time_t T = time(NULL);
    struct tm tm = *localtime(&T);
    snprintf(buf, BUFFERSIZE, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    return buf;
}

char * log_level2str(enum log_level level) {
    static char map [6][9] = {
        "NOT SET",
        "DEBUG",
        "INFO",
        "WARNING",
        "ERROR",
        "CRITICAL"
    };
    return map[level];
}

void set_log_out(FILE * f) {
    log_out = f;
}

void set_log_level(enum log_level level) {
    cur_level = level;
}

int log_f(enum log_level level, const char * fmt, va_list argptr) {
    if (level < cur_level)
        return 0;
    if (log_out == NULL) {
        fputs("log_out variable not set, terminating...\n", stderr);
        return -1;
    }
    char buf[BUFFERSIZE];
    int n = fprintf(log_out,  "[%s] %s: ", timestr(buf), log_level2str(level));
    n += vfprintf(log_out, fmt, argptr);
    return n;
}

int log_not_set(const char * fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    int n = log_f(NOT_SET, fmt, argptr);
    va_end(argptr);
    return n;
}

int log_debug(const char * fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    int n = log_f(DEBUG, fmt, argptr);
    va_end(argptr);
    return n;
}

int log_info(const char * fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    int n = log_f(INFO, fmt, argptr);
    va_end(argptr);
    return n;
}

int log_warning(const char * fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    int n = log_f(WARNING, fmt, argptr);
    va_end(argptr);
    return n;
}

int log_error(const char * fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    int n = log_f(ERROR, fmt, argptr);
    va_end(argptr);
    return n;
}

int log_critical(const char * fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    int n = log_f(CRITICAL, fmt, argptr);
    va_end(argptr);
    return n;
}

