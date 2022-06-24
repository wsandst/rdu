#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "helpers.h"

// Load an array from file
void* load_file(char* filename, size_t* bytes);

// Save an array to file
void save_file(char* filename, void* src, size_t bytes);
