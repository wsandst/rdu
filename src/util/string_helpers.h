#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Does the string start with the string provided? Return 0 if not,
// else return the index
bool str_startswith(char* str, char* match);

// Find first occurance of the provided char
// If none found, return -1
int str_index_of(char* str, char c);