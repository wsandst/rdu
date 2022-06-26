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
#include <getopt.h>

#include "util/helpers.h"

typedef struct Options Options;

// Represents all Make arguments options
struct Options {
    char** files;

    // Display options
    size_t block_size;
    bool human_readable;
    bool summarize;
    bool show_total;
    size_t min_display_size; // Only display files of a certain size
    double min_display_size_percent; // Only display files of a certain percentage of total
    size_t max_depth; // Only display up to a certain depth

    // Search options
    size_t thread_count;
    bool dereference_symlinks; // Dereference all symlinks
    bool dereference_only_arg_symlinks; // Dereference only symlinks in arguments
    bool track_modification_time; // Track total
    bool save_cache; // Save cache to file
    bool use_cache; // Use file cache
};

/**
 * Parse mdu command line arguments
 * ./mdu [-j thread_count] [-B block_size] <file> [files ...] 
 *
 * @return the parsed arguments as an Options struct
 */
Options parse_arguments(int argc, char* argv[]);
