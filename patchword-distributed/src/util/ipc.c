#include "ipc.h"
#include "../include/util.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int create_pipe_pair(PipePair *p) {
    int fds[2];
    if (pipe(fds) < 0) return -1;
    p->read_fd = fds[0];
    p->write_fd = fds[1];
    return 0;
}
