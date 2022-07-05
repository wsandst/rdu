#include "string_helpers.h"

// Find first occurance of the provided char
// If none found, return -1
int str_index_of(char* str, char c) {
    size_t str_length = strlen(str);
    for (size_t i = 0; i < str_length; i++) {
        if (*(str + i) == c) {
            return i;
        }
    }
    return -1;
}

// Does the string start with the string provided? Return 0 if not,
// else return the index
bool str_startswith(char* str, char* match) {
    if (*str == '\0') {
        return false;
    }
    while (*str == *match || *match == '\0' || *str == '\0') {
        if (*match == '\0' || *str == '\0') {
            return true;
        }
        match++;
        str++;
    }
    return false;
}