#ifndef PATCHWORD_IPC_H
#define PATCHWORD_IPC_H

#include <sys/types.h>

typedef struct {
    int read_fd;
    int write_fd;
} PipePair;

int create_pipe_pair(PipePair *p);

#endif
