#include "worker_logic.h"
#include <string.h>
#include <stdio.h>

// FUTURE: Replace this with real dictionary substring matching.
// For now, mimic computation by echoing information back.
void compute_result(const char *payload, char *out, int outsz) {
    snprintf(out, outsz, "processed:%s", payload);
}
