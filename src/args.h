/**
 * This file implements functionality to parse the command line
 * arguments of the minimal make program
 *
 * @file args.h
 * @author wsandst
 */
#pragma once
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "helpers.h"

typedef struct Options Options;

// Represents all Make arguments options
struct Options {
    char** files;
    size_t thread_count;
    size_t block_size;
};

/**
 * Parse mdu command line arguments
 * ./mdu [-j thread_count] [-B block_size] <file> [files ...] 
 *
 * @return the parsed arguments as an Options struct
 */
Options parse_arguments(int argc, char* argv[]);
