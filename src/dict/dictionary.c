#include "dictionary.h"
#include "util_str.h"
#include <string.h>

extern char **get_dictionary(size_t *n);

int find_words_containing(const char *substr, char *out, size_t outsz) {
    size_t n;
    char **dict = get_dictionary(&n);

    out[0] = '\0';

    for (size_t i = 0; i < n; i++) {
        if (contains_substring(dict[i], substr)) {
            if (strlen(out) + strlen(dict[i]) + 2 < outsz) {
                strcat(out, dict[i]);
                strcat(out, " ");
            }
        }
    }

    return 0;
}
