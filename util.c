#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

void to_upper_str(char *s) {
    for (int i = 0; s[i]; i++) {
        s[i] = toupper((unsigned char)s[i]);
    }
}

int valid_letters(const char *s) {
    if (s[0] == '\0') {
        return 0;
    }
    for (int i = 0; s[i]; i++) {
        if (!isalpha((unsigned char)s[i])) {
            return 0;
        }
    }
    return 1;
}

int contains_substring(const char *str, const char *sub) {
    size_t n = strlen(str);
    size_t m = strlen(sub);
    if (m > n) {
        return 0;
    }
    for (size_t i = 0; i <= n - m; i++) {
        int match = 1;
        for (size_t j = 0; j < m; j++) {
            if (tolower((unsigned char)str[i + j]) !=
                tolower((unsigned char)sub[j])) {
                match = 0;
                break;
            }
        }
        if (match) {
            return 1;
        }
    }
    return 0;
}

int equals_ignore_case(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

char *str_dup(const char *s) {
    char *copy = malloc(strlen(s) + 1);
    strcpy(copy, s);
    return copy;
}
