#include "util_ipc.h"
#include "util_fd.h"
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

// Spawns worker with pipes:
// parent -> worker: parent writes, worker reads
// worker -> parent: worker writes, parent reads
int spawn_worker_process(const char *path, char **argv,
                         PipePair *to_worker,
                         PipePair *from_worker,
                         pid_t *child_pid) {

    PipePair in, out;
    if (create_pipe_pair(&in) < 0) return -1;
    if (create_pipe_pair(&out) < 0) return -1;

    pid_t pid = fork();
    if (pid < 0) return -1;

    // ─────────────────────
    // CHILD PROCESS
    // ─────────────────────
    if (pid == 0) {
        // Parent -> worker pipe: child reads
        dup2(in.read_fd, STDIN_FILENO);

        // Worker -> parent pipe: child writes
        dup2(out.write_fd, STDOUT_FILENO);

        // Close unused ends
        close_fd(in.write_fd);
        close_fd(out.read_fd);

        // Exec worker
        execvp(path, argv);

        perror("execvp failed");
        exit(1);
    }

    // ─────────────────────
    // PARENT PROCESS
    // ─────────────────────
    *child_pid = pid;

    // Parent writes to worker
    to_worker->write_fd = in.write_fd;
    to_worker->read_fd = -1;

    // Parent reads from worker
    from_worker->read_fd = out.read_fd;
    from_worker->write_fd = -1;

    // Close child-side fds
    close_fd(in.read_fd);
    close_fd(out.write_fd);

    return 0;
}
