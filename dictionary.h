#ifndef DICTIONARY_H
#define DICTIONARY_H

typedef struct {
    char **words;
    int count;
} Dictionary;

/* Load dictionary file. Returns NULL if file cannot be opened */
Dictionary *load_dictionary(const char *filename);

/* Free dictionary memory */
void free_dictionary(Dictionary *dict);

#endif
