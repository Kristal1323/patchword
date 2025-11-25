#include "protocol.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

static int starts_with(const char *s, const char *prefix) {
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

int parse_message(const char *line, Message *out) {
    // Remove trailing newline
    char buf[MAX_MSG];
    size_t len = strlen(line);

    if (len >= MAX_MSG)
        return -1;

    strcpy(buf, line);
    if (buf[len - 1] == '\n')
        buf[len - 1] = '\0';

    // Reset fields
    out->type = MSG_INVALID;
    out->id = -1;
    out->payload[0] = '\0';

    // ------------- WORKER_READY -------------
    if (strcmp(buf, "WORKER_READY") == 0) {
        out->type = MSG_WORKER_READY;
        return 0;
    }

    // ------------- WORKER_EXITING -------------
    if (strcmp(buf, "WORKER_EXITING") == 0) {
        out->type = MSG_EXITING;
        return 0;
    }

    // ------------- END -------------
    if (strcmp(buf, "END") == 0) {
        out->type = MSG_END;
        return 0;
    }

    // ------------- TASK <id> <payload> -------------
    if (starts_with(buf, "TASK ")) {
        const char *p = buf + 5;
        char *endptr;

        long id = strtol(p, &endptr, 10);
        if (endptr == p || id < 0)
            return -1;

        out->id = (int)id;

        // Expecting space then payload
        if (*endptr == ' ') {
            strncpy(out->payload, endptr + 1, MAX_PAYLOAD - 1);
            out->payload[MAX_PAYLOAD - 1] = '\0';
        } else {
            out->payload[0] = '\0';
        }

        out->type = MSG_TASK;
        return 0;
    }

    // ------------- RESULT <id> <payload> -------------
    if (starts_with(buf, "RESULT ")) {
        const char *p = buf + 7;
        char *endptr;
        long id = strtol(p, &endptr, 10);
        if (endptr == p || id < 0)
            return -1;

        out->id = id;

        if (*endptr == ' ') {
            strncpy(out->payload, endptr + 1, MAX_PAYLOAD - 1);
            out->payload[MAX_PAYLOAD - 1] = '\0';
        } else {
            out->payload[0] = '\0';
        }

        out->type = MSG_RESULT;
        return 0;
    }

    // ------------- ERROR <msg> -------------
    if (starts_with(buf, "ERROR ")) {
        const char *msg = buf + 6;
        strncpy(out->payload, msg, MAX_PAYLOAD - 1);
        out->payload[MAX_PAYLOAD - 1] = '\0';
        out->type = MSG_ERROR;
        return 0;
    }

    return -1;
}

void format_message(char *buf, size_t n, const Message *msg) {
    switch (msg->type) {

        case MSG_WORKER_READY:
            snprintf(buf, n, "WORKER_READY\n");
            return;

        case MSG_EXITING:
            snprintf(buf, n, "WORKER_EXITING\n");
            return;

        case MSG_END:
            snprintf(buf, n, "END\n");
            return;

        case MSG_ERROR:
            snprintf(buf, n, "ERROR %s\n", msg->payload);
            return;

        case MSG_TASK:
            snprintf(buf, n, "TASK %d %s\n", msg->id, msg->payload);
            return;

        case MSG_RESULT:
            snprintf(buf, n, "RESULT %d %s\n", msg->id, msg->payload);
            return;

        default:
            snprintf(buf, n, "ERROR invalid\n");
    }
}
