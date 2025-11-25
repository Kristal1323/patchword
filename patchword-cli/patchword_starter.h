#ifndef PATCHWORD_STARTER_H
#define PATCHWORD_STARTER_H

#include "dictionary.h"

/*
 * Returns a usable starter word of desired length.
 * wordLen = 3 or 4 => specific length
 * wordLen = 0 => randomly choose 3 or 4
 */
const char *get_patchword_starter_word(Dictionary *dict, unsigned int wordLen);

#endif
