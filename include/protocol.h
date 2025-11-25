#ifndef PATCHWORD_PROTOCOL_H
#define PATCHWORD_PROTOCOL_H

#include <stddef.h>
#include <stdio.h>

#define MAX_MSG 512
#define MAX_PAYLOAD 400

// Worker protocol message types 
typedef enum {
    MSG_WORKER_READY,  // WORKER_READY\n
    MSG_TASK,          // TASK <id> <payload>\n
    MSG_RESULT,        // RESULT <id> <payload>\n
    MSG_END,           // END\n
    MSG_ERROR,         // ERROR <msg>\n
    MSG_EXITING,       // WORKER_EXITING\n
    MSG_INVALID
} MessageType;

typedef struct {
    MessageType type;
    int id;                          // For TASK/RESULT
    char payload[MAX_PAYLOAD];       // extra data
} Message;

// Parse a received line -> structured message
int parse_message(const char *line, Message *out);

// Format structured message -> line
void format_message(char *buf, size_t n, const Message *msg);

#endif

