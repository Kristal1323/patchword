#include "util.h"
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

#define COL_RESET  "\033[0m"
#define COL_INFO   "\033[36m"
#define COL_WARN   "\033[33m"
#define COL_ERR    "\033[31m"
#define COL_WORKER "\033[35m"

void log_info(const char *fmt, ...) {
    va_list ap;
    fprintf(stderr, COL_INFO "[INFO] " COL_RESET);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}

void log_warn(const char *fmt, ...) {
    va_list ap;
    fprintf(stderr, COL_WARN "[WARN] " COL_RESET);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}

void log_error(const char *fmt, ...) {
    va_list ap;
    fprintf(stderr, COL_ERR "[ERROR] " COL_RESET);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}

void log_worker(int wid, const char *fmt, ...) {
    va_list ap;
    fprintf(stderr, COL_WORKER "[WORKER %d] " COL_RESET, wid);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}

// Reads a line ending in '\n'
ssize_t safe_readline(int fd, char *buf, size_t n) {
    size_t pos = 0;
    while (pos < n - 1) {
        ssize_t r = read(fd, &buf[pos], 1);
        if (r <= 0) return r;
        if (buf[pos++] == '\n') break;
    }
    buf[pos] = '\0';
    return pos;
}

ssize_t safe_writeline(int fd, const char *buf) {
    size_t len = strlen(buf);
    return write(fd, buf, len);
}
