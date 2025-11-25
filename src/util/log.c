#include "util_log.h"
#include <stdio.h>
#include <stdarg.h>

static void vlog(const char *prefix, const char *fmt, va_list ap) {
    fprintf(stderr, "%s", prefix);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
}

void log_info(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vlog("[INFO] ", fmt, ap);
    va_end(ap);
}

void log_warn(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vlog("[WARN] ", fmt, ap);
    va_end(ap);
}

void log_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vlog("[ERROR] ", fmt, ap);
    va_end(ap);
}

void log_worker(int pid, const char *fmt, ...) {
    va_list ap;
    fprintf(stderr, "[WORKER %d] ", pid);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}
