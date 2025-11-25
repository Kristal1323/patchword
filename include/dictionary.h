#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stddef.h>

int load_dictionary(const char *path);
char **get_dictionary(size_t *out_n);
int find_words_containing(const char *substr, char *out, size_t outsz);

#endif
