#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "patchword_game.h"
#include "patchword_starter.h"
#include "util_str.h"
#include "dictionary.h"

#define MAX_GUESSES 5
#define BUF 256

/* -------------------------------
   Validate manually provided starter
   ------------------------------- */
static int valid_starter(const char *w) {
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

/* -------------------------------
   Dictionary membership test
   ------------------------------- */
static int dict_contains_word(const char *word) {
    size_t nwords;
    char **words = get_dictionary(&nwords);

    for (size_t i = 0; i < nwords; i++) {
        if (equals_ignore_case(words[i], word)) {
            return 1;
        }
    }
    return 0;
}

/* =============================================================
   Main Game Function
   ============================================================= */
int patchword_game_run(int argc, char **argv) {

    char *starterWord = NULL;
    int lenOption = 3;      // default to 3-letter starters
    int lenProvided = 0;

    /* ----------------------------------------
       Parse CLI arguments
       ---------------------------------------- */
    for (int i = 1; i < argc; i++) {

        if (!strcmp(argv[i], "--start")) {
            if (starterWord || lenOption) {
                fprintf(stderr,
                        "Usage: patchword [--start word | --len 3|4]\n");
                return 1;
            }
            if (i + 1 >= argc) {
                fprintf(stderr,
                        "Usage: patchword [--start word | --len 3|4]\n");
                return 1;
            }
            starterWord = argv[++i];

        } else if (!strcmp(argv[i], "--len")) {
            if (starterWord || lenProvided) {
                fprintf(stderr,
                        "Usage: patchword [--start word | --len 3|4]\n");
                return 1;
            }
            if (i + 1 >= argc) {
                fprintf(stderr,
                        "Usage: patchword [--start word | --len 3|4]\n");
                return 1;
            }
            lenOption = atoi(argv[++i]);
            if (lenOption != 3 && lenOption != 4) {
                fprintf(stderr,
                        "Usage: patchword [--start word | --len 3|4]\n");
                return 1;
            }
            lenProvided = 1;

        } else {
            fprintf(stderr,
                    "Usage: patchword [--start word | --len 3|4]\n");
            return 1;
        }
    }

    /* ----------------------------------------
       Validate provided starter
       ---------------------------------------- */
    if (starterWord && !valid_starter(starterWord)) {
        fprintf(stderr, "patchword: invalid starter word\n");
        return 2;
    }

    /* ----------------------------------------
       Determine starter word
       ---------------------------------------- */
    char starter[10];

    if (starterWord) {
        strcpy(starter, starterWord);
        to_upper_str(starter);
    } else {
        const char *s = generate_starter(lenOption);
        strcpy(starter, s);
        to_upper_str(starter);
    }

    printf("Welcome to Patchword!\n");
    printf("The starter word is: %s\n", starter);
    printf("Enter words containing this word.\n");

    /* ----------------------------------------
       Guess collection
       ---------------------------------------- */
    char *validGuesses[MAX_GUESSES];
    int validCount = 0;

    for (int guess = 1; guess <= MAX_GUESSES; guess++) {

        printf("Enter guess %d:\n", guess);

        char buffer[BUF];
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            break;
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        /* Validation loop */
        while (1) {

            if (!valid_letters(buffer)) {
                printf("Guesses must contain only letters - try again.\n");

            } else if (!contains_substring(buffer, starter)) {
                printf("Guesses must contain the starter word - try again.\n");

            } else if (equals_ignore_case(buffer, starter)) {
                printf("Guesses can't be the starter word - try again.\n");

            } else if (!dict_contains_word(buffer)) {
                printf("Guess not found in dictionary - try again.\n");

            } else {
                /* Check duplicates */
                int dup = 0;
                for (int i = 0; i < validCount; i++) {
                    if (equals_ignore_case(validGuesses[i], buffer)) {
                        dup = 1;
                        break;
                    }
                }
                if (dup) {
                    printf("You've already guessed that word - try again.\n");
                } else {
                    validGuesses[validCount++] = str_dup(buffer);
                    break;
                }
            }

            printf("Enter guess %d:\n", guess);
            if (!fgets(buffer, sizeof(buffer), stdin)) goto endgame;
            buffer[strcspn(buffer, "\n")] = '\0';
        }

        if (validCount == MAX_GUESSES)
            break;
    }

endgame:

    if (validCount == 0) {
        return 4;
    }

    printf("\n");

    /* ----------------------------------------
       Sum total length
       ---------------------------------------- */
    int totalLen = 0;
    for (int i = 0; i < validCount; i++) {
        totalLen += strlen(validGuesses[i]);
    }
    printf("Total length of words found: %d\n", totalLen);

    /* ----------------------------------------
       Longest guesses by player
       ---------------------------------------- */
    size_t maxGuessLen = 0;
    for (int i = 0; i < validCount; i++) {
        size_t L = strlen(validGuesses[i]);
        if (L > maxGuessLen) maxGuessLen = L;
    }

    printf("Longest word(s) found:\n");
    for (int i = 0; i < validCount; i++) {
        if (strlen(validGuesses[i]) == maxGuessLen) {
            char *u = str_dup(validGuesses[i]);
            to_upper_str(u);
            printf("%s (%zu)\n", u, maxGuessLen);
            free(u);
        }
    }

    /* ----------------------------------------
       Longest possible in dictionary
       ---------------------------------------- */
    size_t dictCount;
    char **dict = get_dictionary(&dictCount);

    int bestLen = 0;
    for (size_t i = 0; i < dictCount; i++) {
        if (contains_substring(dict[i], starter)) {
            int L = strlen(dict[i]);
            if (L > bestLen) bestLen = L;
        }
    }

    printf("Longest word(s) possible:\n");
    for (size_t i = 0; i < dictCount; i++) {
        if ((int)strlen(dict[i]) == bestLen &&
            contains_substring(dict[i], starter)) {

            char *u = str_dup(dict[i]);
            to_upper_str(u);
            printf("%s (%d)\n", u, bestLen);
            free(u);
        }
    }

    for (int i = 0; i < validCount; i++)
        free(validGuesses[i]);

    return 0;
}
