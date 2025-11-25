#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dictionary.h"

#define MAX_WORD_LEN 51

Dictionary *load_dictionary(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return NULL;
    }

    Dictionary *dict = malloc(sizeof(Dictionary));
    dict->words = NULL;
    dict->count = 0;

    char buffer[MAX_WORD_LEN + 5];
    while (fgets(buffer, sizeof(buffer), fp)) {
        size_t len = strcspn(buffer, "\n");
        buffer[len] = '\0';

        char *copy = malloc(len + 1);
        strcpy(copy, buffer);

        dict->words = realloc(dict->words, sizeof(char*) * (dict->count + 1));
        dict->words[dict->count] = copy;
        dict->count++;
    }

    fclose(fp);
    return dict;
}

void free_dictionary(Dictionary *dict) {
    if (!dict) {
        return;
    }
    for (int i = 0; i < dict->count; i++) {
        free(dict->words[i]);
    }
    free(dict->words);
    free(dict);
}
