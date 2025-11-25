#include "patchword_starter.h"
#include "dictionary.h"
#include "util_str.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_STARTERS 10000
#define MAX_LEN 32

// Utility: check if word is alphabetical
static int is_alpha_word(const char *w) {
    for (int i = 0; w[i]; i++) {
        if (!isalpha((unsigned char)w[i])) {
            return 0;
        }
    }
    return 1;
}

// Utility: check vowel/consonant balance
static int has_vowel(const char *w) {
    for (int i = 0; w[i]; i++) {
        char c = tolower(w[i]);
        if (c=='a'||c=='e'||c=='i'||c=='o'||c=='u') return 1;
    }
    return 0;
}

// Score a substring by heuristics
static int score_substring(const char *sub, char **dict, size_t n) {
    int score = 0;
    int len = strlen(sub);

    // Vowel check
    if (has_vowel(sub)) score += 4;

    // Avoid weird letters
    for (int i = 0; i < len; i++) {
        char c = tolower(sub[i]);
        if (c == 'q' || c == 'z' || c == 'x') score -= 1;
    }

    // Count dictionary occurrences
    int occurrences = 0;
    for (size_t i = 0; i < n; i++) {
        if (strstr(dict[i], sub)) {
            occurrences++;
        }
        if (occurrences >= 40) break; // cap scoring cost
    }

    score += occurrences / 2;    // main scoring factor

    return score;
}

const char *generate_starter(int length) {
    static char best[8];
    best[0] = '\0';

    size_t n;
    char **dict = get_dictionary(&n);

    typedef struct {
        char s[8];
        int score;
    } Candidate;

    static Candidate list[MAX_STARTERS];
    int count = 0;

    // Build substring candidates from dictionary words
    for (size_t i = 0; i < n && count < MAX_STARTERS; i++) {

        const char *w = dict[i];
        int L = strlen(w);

        if (L < length) continue;
        if (!is_alpha_word(w)) continue;

        // sliding window substrings of size length
        for (int k = 0; k <= L - length; k++) {
            char sub[8];
            memcpy(sub, w + k, length);
            sub[length] = '\0';

            // Validate substring
            if (!is_alpha_word(sub)) continue;
            if (!has_vowel(sub)) continue;     // require at least one vowel
            if (strlen(sub) != length) continue;

            // Score it
            int score = score_substring(sub, dict, n);
            if (score <= 0) continue; // ignore trash

            // Store candidate
            strcpy(list[count].s, sub);
            list[count].score = score;
            count++;

            if (count >= MAX_STARTERS) break;
        }
    }

    if (count == 0) {
        strcpy(best, "ing"); // fallback
        return best;
    }

    // Pick one of the top-scoring candidates
    int bestScore = -9999;
    for (int i = 0; i < count; i++) {
        if (list[i].score > bestScore) {
            bestScore = list[i].score;
            strcpy(best, list[i].s);
        }
    }

    // Randomize among the top 10%
    srand((unsigned)time(NULL));

    int threshold = bestScore * 0.9;
    int poolSize = 0;

    static Candidate pool[MAX_STARTERS];

    for (int i = 0; i < count; i++) {
        if (list[i].score >= threshold) {
            pool[poolSize++] = list[i];
        }
    }

    if (poolSize > 0) {
        int idx = rand() % poolSize;
        strcpy(best, pool[idx].s);
    }

    // Convert to uppercase for display
    for (int i = 0; best[i]; i++) {
        best[i] = toupper(best[i]);
    }

    return best;
}
