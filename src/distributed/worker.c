#include "worker.h"
#include "util_fd.h"
#include "util_io.h"
#include "util_ipc.h"
#include "util_log.h"
#include "util_signals.h"
#include "protocol.h"
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
    char buf[MAX_MSG];
    while (1) {
        ssize_t n = safe_readline(STDIN_FILENO, buf, sizeof(buf));
        if (n <= 0) {
            log_worker(getpid(), "EOF from coordinator, exiting");
            break;
        }

        Message msg;
        if (parse_message(buf, &msg) < 0) {
            log_worker(getpid(), "Invalid message: %s", buf);
            continue;
        }

        switch (msg.type) {
        case MSG_TASK: {
            log_worker(getpid(), "Received TASK %d (%s)", msg.id, msg.payload);

            // Compute result
            char resultbuf[MAX_PAYLOAD];
            compute_result(msg.payload, resultbuf, sizeof(resultbuf));

            Message r;
            r.type = MSG_RESULT;
            r.id = msg.id;
            strncpy(r.payload, resultbuf, MAX_PAYLOAD - 1);
            r.payload[MAX_PAYLOAD - 1] = '\0';

            char outbuf[MAX_MSG];
            format_message(outbuf, sizeof(outbuf), &r);
            safe_writeline(STDOUT_FILENO, outbuf);
            break;
        }

        case MSG_END:
            log_worker(getpid(), "Received END, shutting down");
            goto exit_now;

        case MSG_ERROR:
            log_worker(getpid(), "Received ERROR: %s", msg.payload);
            break;

        default:
            log_worker(getpid(), "Unknown message type %d", msg.type);
        }
    }

exit_now: {
    // Notify coordinator we're exiting
    Message exitmsg;
    exitmsg.type = MSG_EXITING;
    char exitbuf[MAX_MSG];
    format_message(exitbuf, sizeof(exitbuf), &exitmsg);
    safe_writeline(STDOUT_FILENO, exitbuf);

    log_worker(getpid(), "Worker exiting");
    return 0;
}
}
