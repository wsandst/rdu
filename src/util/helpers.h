/**
 * This file implements various helpers to 
 * simplify error handling and such
 *
 * @file helpers.h
 * @author William Sandström
 */
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Convert C string to unsigned (size_t) integer
 * Prints error_message if string is not a number
 * or 0 and below
 */
size_t checked_unsigned_atoi(char* str, char* error_message);

/**
 * Performs calloc with error checking
 */
void* checked_calloc(size_t length, size_t bytes);

/**
 * Performs malloc with error checking
 */
void* checked_malloc(size_t length, size_t bytes);

/**
 * Performs realloc with error checking
 */
void* checked_realloc(void* ptr, size_t length, size_t bytes);

/**
 * Print error message to stderr and exit with EXIT_FAILURE
 */
void stderr_and_exit(char* error_message);

/**
 * Print perror and exit with EXIT_FAILURE
 */
void perror_and_exit(char* error_message);

/**
 * Combine two paths with a / inbetween
 * Same as sprintf(buffer, "%s/%s", path1, path2)
 */
void combine_paths(char* restrict buffer, const char* restrict path1, char* restrict path2);
