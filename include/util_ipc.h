#ifndef UTIL_IPC_H
#define UTIL_IPC_H

#include <sys/types.h>

typedef struct {
    int read_fd;
    int write_fd;
} PipePair;

int create_pipe_pair(PipePair *p);
int spawn_worker_process(const char *path, char **argv,
                         PipePair *to_worker,
                         PipePair *from_worker,
                         pid_t *child_pid);

#endif
