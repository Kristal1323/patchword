#ifndef PATCHWORD_PROTOCOL_H
#define PATCHWORD_PROTOCOL_H

#include <stddef.h>  
#include <stdio.h>    

// Message buffer sizes
#define MAX_MSG 512

// Protocol message types
typedef enum {
    MSG_WORKER_READY,
    MSG_TASK,
    MSG_RESULT,
    MSG_END,
    MSG_ERROR,
    MSG_EXITING,
    MSG_INVALID
} MessageType;

typedef struct {
    MessageType type;
    int id;
    char payload[400];
} Message;

// Parsing + serializing
int parse_message(const char *line, Message *out);
void format_message(char *buf, size_t n, const Message *msg);

#endif
