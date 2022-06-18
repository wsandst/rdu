/**
 * This file implements various helpers to 
 * simplify error handling and such
 *
 * @file helpers.c
 * @author William Sandström (c20wsm)
 * @date ADD ON HANDIN
 */
#include "helpers.h"

/**
 * Convert C string to unsigned (size_t) integer
 * Prints error_message if string is not a number
 * or 0 and below
 */
size_t checked_unsigned_atoi(char* str, char* error_message) {
    int val = atoi(str);
    if (val <= 0) {
        stderr_and_exit(error_message);
    }
    return (size_t) val;
}

// ============= Error checked allocation ================

/**
 * Performs calloc with error checking
 */
void* checked_calloc(size_t length, size_t bytes) {
    void* ptr = calloc(length, bytes);
    if (ptr == NULL) {
        perror_and_exit("Calloc error");
    }
    return ptr;
}

/**
 * Performs malloc with error checking
 */
void* checked_malloc(size_t length, size_t bytes) {
    void* ptr = malloc(length * bytes);
    if (ptr == NULL) {
        perror_and_exit("Malloc error");
    }
    return ptr;
}

/**
 * Performs realloc with error checking
 */
void* checked_realloc(void* ptr, size_t length, size_t bytes) {
    ptr = realloc(ptr, length * bytes);
    if (ptr == NULL) {
        perror_and_exit("Realloc error");
    }
    return ptr;
}

// ================ Error messages and quit ================

/**
 * Print error message to stderr and exit with EXIT_FAILURE
 */
void stderr_and_exit(char* error_message) {
    char str_buf[256];
    snprintf(str_buf, 255, "Error: %s\n", error_message);
    fprintf(stderr, "%s", str_buf);
    exit(EXIT_FAILURE);
}

/**
 * Print perror and exit with EXIT_FAILURE
 */
void perror_and_exit(char* error_message) {
    perror(error_message);
    exit(EXIT_FAILURE);
}