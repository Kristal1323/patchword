#include "worker.h"
#include "protocol.h"
#include "util.h"
#include "worker_logic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int worker_main(int argc, char **argv) {
    (void)argc; (void)argv;

    log_worker(getpid(), "Worker process started");

    // Send WORKER_READY to coordinator
    char line[MAX_MSG];
    Message out;
    out.type = MSG_WORKER_READY;
    format_message(line, sizeof(line), &out);
    safe_writeline(STDOUT_FILENO, line);

    // Main loop: read messages from coordinator
}