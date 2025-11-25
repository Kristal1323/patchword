#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dictionary.h"
#include "patchword_starter.h"
#include "util.h"

#define DEFAULT_DICT "/usr/share/dict/words"
#define MAX_GUESSES 5
#define BUF 256

/* Validate starter word (letters only, length 3 or 4) */
int valid_starter(const char *w) {
    int len = strlen(w);
    if (len != 3 && len != 4) {
        return 0;
    }
    for (int i = 0; i < len; i++) {
        if (!isalpha((unsigned char)w[i])) {
            return 0;
        }
    }
    return 1;
}

/* Check dictionary contains a word (case-insensitive) */
int dict_contains(Dictionary *dict, const char *word) {
    for (int i = 0; i < dict->count; i++) {
        if (equals_ignore_case(dict->words[i], word)) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    char *starterWord = NULL;
    int lenOption = 0;
    char *dictFile = NULL;

    /* --- Command line parsing --- */
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--start")) {
            if (starterWord || lenOption) {
                fprintf(stderr,
                        "Usage: patchword [--start starter-word | --len length] "
                        "[--dictionary filename]\n");
                return 1;
            }
            if (i + 1 >= argc) {
                fprintf(stderr,
                        "Usage: patchword [--start starter-word | --len length] "
                        "[--dictionary filename]\n");
                return 1;
            }
            starterWord = argv[++i];

        } else if (!strcmp(argv[i], "--len")) {
            if (starterWord || lenOption) {
                fprintf(stderr,
                        "Usage: patchword [--start starter-word | --len length] "
                        "[--dictionary filename]\n");
                return 1;
            }
            if (i + 1 >= argc) {
                fprintf(stderr,
                        "Usage: patchword [--start starter-word | --len length] "
                        "[--dictionary filename]\n");
                return 1;
            }
            lenOption = atoi(argv[++i]);
            if (lenOption != 3 && lenOption != 4) {
                fprintf(stderr,
                        "Usage: patchword [--start starter-word | --len length] "
                        "[--dictionary filename]\n");
                return 1;
            }

        } else if (!strcmp(argv[i], "--dictionary")) {
            if (i + 1 >= argc) {
                fprintf(stderr,
                        "Usage: patchword [--start starter-word | --len length] "
                        "[--dictionary filename]\n");
                return 1;
            }
            dictFile = argv[++i];

        } else {
            fprintf(stderr,
                    "Usage: patchword [--start starter-word | --len length] "
                    "[--dictionary filename]\n");
            return 1;
        }
    }

    /* --- Starter validation if provided --- */
    if (starterWord && !valid_starter(starterWord)) {
        fprintf(stderr, "patchword: invalid starter word\n");
        return 2;
    }

    if (!dictFile) {
        dictFile = (char *)DEFAULT_DICT;
    }

    /* --- Load dictionary --- */
    Dictionary *dict = load_dictionary(dictFile);
    if (!dict) {
        fprintf(stderr, "patchword: dictionary file \"%s\" cannot be opened\n",
                dictFile);
        return 3;
    }

    /* --- Determine starter word --- */
    char starter[10];
    if (starterWord) {
        strcpy(starter, starterWord);
    } else {
        const char *s = get_patchword_starter_word(dict, lenOption);
        strcpy(starter, s);
    }

    /* Convert to uppercase for printing */
    char starterUpper[10];
    strcpy(starterUpper, starter);
    to_upper_str(starterUpper);

    printf("Welcome to Patchword!\n");
    printf("The starter word is: %s\n", starterUpper);
    printf("Enter words containing this word.\n");

    /* --- Guess loop --- */
    char *validGuesses[MAX_GUESSES];
    int validCount = 0;

    for (int i = 1; i <= MAX_GUESSES; i++) {
        printf("Enter guess %d:\n", i);

        char buffer[BUF];
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            break;
        }

        size_t len = strcspn(buffer, "\n");
        buffer[len] = '\0';

        /* Re-prompt until valid or EOF */
        while (1) {

            if (!valid_letters(buffer)) {
                printf("Guesses must contain only letters - try again.\n");
                printf("Enter guess %d:\n", i);
                if (!fgets(buffer, sizeof(buffer), stdin)) {
                    goto endgame;
                }
                len = strcspn(buffer, "\n");
                buffer[len] = '\0';
                continue;
            }

            if (!contains_substring(buffer, starter)) {
                printf("Guesses must contain the starter word - try again.\n");
                printf("Enter guess %d:\n", i);
                if (!fgets(buffer, sizeof(buffer), stdin)) {
                    goto endgame;
                }
                len = strcspn(buffer, "\n");
                buffer[len] = '\0';
                continue;
            }

            if (equals_ignore_case(buffer, starter)) {
                printf("Guesses can't be the starter word - try again.\n");
                printf("Enter guess %d:\n", i);
                if (!fgets(buffer, sizeof(buffer), stdin)) {
                    goto endgame;
                }
                len = strcspn(buffer, "\n");
                buffer[len] = '\0';
                continue;
            }

            if (!dict_contains(dict, buffer)) {
                printf("Guess not found in dictionary - try again.\n");
                printf("Enter guess %d:\n", i);
                if (!fgets(buffer, sizeof(buffer), stdin)) {
                    goto endgame;
                }
                len = strcspn(buffer, "\n");
                buffer[len] = '\0';
                continue;
            }

            int already = 0;
            for (int k = 0; k < validCount; k++) {
                if (equals_ignore_case(validGuesses[k], buffer)) {
                    already = 1;
                    break;
                }
            }
            if (already) {
                printf("You've already guessed that word - try again.\n");
                printf("Enter guess %d:\n", i);
                if (!fgets(buffer, sizeof(buffer), stdin)) {
                    goto endgame;
                }
                len = strcspn(buffer, "\n");
                buffer[len] = '\0';
                continue;
            }

            /* Valid guess */
            validGuesses[validCount++] = str_dup(buffer);
            break;
        }

        if (validCount == MAX_GUESSES) {
            break;
        }
    }

endgame:

    if (validCount == 0) {
        free_dictionary(dict);
        return 4;
    }

    printf("\n");

    /* --- Total length --- */
    int total = 0;
    for (int i = 0; i < validCount; i++) {
        total += strlen(validGuesses[i]);
    }
    printf("Total length of words found: %d\n", total);

    /* --- Longest found guesses --- */
    int maxGuessLen = 0;
    for (int i = 0; i < validCount; i++) {
        int L = strlen(validGuesses[i]);
        if (L > maxGuessLen) {
            maxGuessLen = L;
        }
    }

    printf("Longest word(s) found:\n");
    for (int i = 0; i < validCount; i++) {
        if ((int)strlen(validGuesses[i]) == maxGuessLen) {
            char *u = str_dup(validGuesses[i]);
            to_upper_str(u);
            printf("%s (%d)\n", u, maxGuessLen);
            free(u);
        }
    }

    /* --- Longest possible in dictionary --- */
    int bestLen = 0;
    for (int i = 0; i < dict->count; i++) {
        if (contains_substring(dict->words[i], starter)) {
            int L = strlen(dict->words[i]);
            if (L > bestLen) {
                bestLen = L;
            }
        }
    }

    printf("Longest word(s) possible:\n");
    for (int i = 0; i < dict->count; i++) {
        if (contains_substring(dict->words[i], starter)) {
            if ((int)strlen(dict->words[i]) == bestLen) {
                char *u = str_dup(dict->words[i]);
                to_upper_str(u);
                printf("%s (%d)\n", u, bestLen);
                free(u);
            }
        }
    }

    for (int i = 0; i < validCount; i++) {
        free(validGuesses[i]);
    }

    free_dictionary(dict);

    return 0;
}
