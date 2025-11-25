#ifndef UTIL_H
#define UTIL_H

/* Convert a string to uppercase (in place) */
void to_upper_str(char *s);

/* Return 1 if guess contains only letters */
int valid_letters(const char *s);

/* Return 1 if substring appears in string (case-insensitive) */
int contains_substring(const char *str, const char *sub);

/* Return 1 if both strings match ignoring case */
int equals_ignore_case(const char *a, const char *b);

/* Duplicate string to new allocated memory */
char *str_dup(const char *s);

#endif
