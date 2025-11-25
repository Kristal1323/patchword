#include "util_io.h"
#include <unistd.h>
#include <string.h>

ssize_t safe_readline(int fd, char *buf, size_t bufsize) {
    size_t pos = 0;

    while (pos < bufsize - 1) {
        char c;
        ssize_t r = read(fd, &c, 1);

        if (r <= 0) {
            if (pos == 0) return -1;  // EOF or error before reading
            break;
        }

        if (c == '\n') break;

        buf[pos++] = c;
    }

    buf[pos] = '\0';
    return pos;
}

void safe_writeline(int fd, const char *line) {
    write(fd, line, strlen(line));
    write(fd, "\n", 1);
}
