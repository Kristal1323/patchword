#ifndef PATCHWORD_UTIL_H
#define PATCHWORD_UTIL_H

#include <stdio.h>
#include <sys/types.h>

// ----- Color Logging -----
void log_info(const char *fmt, ...);
void log_warn(const char *fmt, ...);
void log_error(const char *fmt, ...);
void log_worker(int wid, const char *fmt, ...);

// ----- Safe pipe I/O -----
ssize_t safe_readline(int fd, char *buf, size_t n);
ssize_t safe_writeline(int fd, const char *buf);

// ----- File Descripter (FD) helpers -----
int close_fd(int fd);
int set_fd_nonblocking(int fd);
int set_fd_blocking(int fd);

#endif
