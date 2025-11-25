#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "patchword_starter.h"
#include "util.h"

static char **starters = NULL;
static int starterCount = 0;
static char starterBuf[5];
static int seeded = 0;

static int is_alpha_str(const char *s) {
    for (int i = 0; s[i]; i++) {
        if (!isalpha((unsigned char)s[i])) {
            return 0;
        }
    }
    return 1;
}

/* Build the starter list only once */
static void build_starter_list(Dictionary *dict) {
    if (starters != NULL) {
        return; // already built
    }

    starters = NULL;
    starterCount = 0;

    for (int i = 0; i < dict->count; i++) {
        char *w = dict->words[i];
        int L = strlen(w);

        if (L < 3)
            continue;

        if (!is_alpha_str(w))
            continue;

        /* Add 3-letter substrings */
        for (int j = 0; j <= L - 3; j++) {
            char sub[4];
            strncpy(sub, w + j, 3);
            sub[3] = '\0';
            starters = realloc(starters, sizeof(char*) * (starterCount + 1));
            starters[starterCount++] = str_dup(sub);
        }

        /* Add 4-letter substrings */
        if (L >= 4) {
            for (int j = 0; j <= L - 4; j++) {
                char sub[5];
                strncpy(sub, w + j, 4);
                sub[4] = '\0';
                starters = realloc(starters, sizeof(char*) * (starterCount + 1));
                starters[starterCount++] = str_dup(sub);
            }
        }
    }
}

const char *get_patchword_starter_word(Dictionary *dict, unsigned int wordLen) {
    if (!seeded) {
        srand(time(NULL));
        seeded = 1;
    }

    if (wordLen != 3 && wordLen != 4) {
        wordLen = (rand() % 2) ? 3 : 4;
    }

    build_starter_list(dict);

    while (1) {
        int idx = rand() % starterCount;
        if ((int)strlen(starters[idx]) == (int)wordLen) {
            strcpy(starterBuf, starters[idx]);
            return starterBuf;
        }
    }
}
