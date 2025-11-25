#ifndef UTIL_IO_H
#define UTIL_IO_H

#include <unistd.h>
#include <stddef.h>

ssize_t safe_readline(int fd, char *buf, size_t bufsize);
void safe_writeline(int fd, const char *line);

#endif
