#include "util_str.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char *str_dup(const char *s) {
    char *p = malloc(strlen(s) + 1);
    strcpy(p, s);
    return p;
}

void to_upper_str(char *s) {
    for (int i = 0; s[i]; i++) {
        s[i] = toupper((unsigned char)s[i]);
    }
}

int valid_letters(const char *s) {
    for (int i = 0; s[i]; i++) {
        if (!isalpha((unsigned char)s[i])) {
            return 0;
        }
    }
    return 1;
}

int equals_ignore_case(const char *a, const char *b) {
    if (strlen(a) != strlen(b)) return 0;

    for (size_t i = 0; i < strlen(a); i++) {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])) {
            return 0;
        }
    }
    return 1;
}

int contains_substring(const char *haystack, const char *needle) {
    size_t N = strlen(needle);
    size_t H = strlen(haystack);

    if (N > H) return 0;

    for (size_t i = 0; i <= H - N; i++) {
        int match = 1;
        for (size_t j = 0; j < N; j++) {
            if (tolower((unsigned char)haystack[i + j]) !=
                tolower((unsigned char)needle[j])) {
                match = 0;
                break;
            }
        }
        if (match) return 1;
    }
    return 0;
}
