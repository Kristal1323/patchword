#include "worker_logic.h"
#include "dictionary.h"
#include <string.h>

void compute_result(const char *substr, char *out, size_t outsz) {
    find_words_containing(substr, out, outsz);
}
