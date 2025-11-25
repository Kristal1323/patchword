#ifndef PATCHWORD_UTIL_H
#define PATCHWORD_UTIL_H

#include <stdio.h>

// Color logging
void log_info(const char *fmt, ...);
void log_warn(const char *fmt, ...);
void log_error(const char *fmt, ...);
void log_worker(int wid, const char *fmt, ...);

ssize_t safe_readline(int fd, char *buf, size_t n);
ssize_t safe_writeline(int fd, const char *buf);

#endif
