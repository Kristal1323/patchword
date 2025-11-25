#ifndef PATCHWORD_WORKER_LOGIC_H
#define PATCHWORD_WORKER_LOGIC_H

// Worker computes a "result" string derived from the incoming task payload.
// For Patchword, payload is the starter fragment. Worker generates all valid words
// containing that substring. Here we simulate with placeholder logic.
void compute_result(const char *payload, char *out, int outsz);

#endif
