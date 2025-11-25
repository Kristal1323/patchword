#ifndef UTIL_STR_H
#define UTIL_STR_H

// Allocate a duplicate string (malloc)
char *str_dup(const char *s);

// Convert string to uppercase (in-place)
void to_upper_str(char *s);

// Check if word contains only letters
int valid_letters(const char *s);

// Case-insensitive compare
int equals_ignore_case(const char *a, const char *b);

// Check if haystack contains needle (case-insensitive)
int contains_substring(const char *haystack, const char *needle);

#endif
