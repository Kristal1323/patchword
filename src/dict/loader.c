#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LEN 64

static char **dict = NULL;
static size_t dict_count = 0;

int load_dictionary(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;

    size_t cap = 50000;
    dict = malloc(cap * sizeof(char*));

    char buf[MAX_WORD_LEN];
    while (fgets(buf, sizeof(buf), f)) {
        // Strip both newline and any carriage return for Windows-formatted files
        buf[strcspn(buf, "\r\n")] = 0;

        if (dict_count == cap) {
            cap *= 2;
            dict = realloc(dict, cap * sizeof(char*));
        }

        dict[dict_count] = malloc(strlen(buf) + 1);
        strcpy(dict[dict_count], buf);

        dict_count++;
    }

    fclose(f);
    return 0;
}

char **get_dictionary(size_t *out_n) {
    *out_n = dict_count;
    return dict;
}
