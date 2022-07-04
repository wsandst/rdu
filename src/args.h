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
#include <math.h>
#include <limits.h>

#include "util/helpers.h"

typedef struct Options Options;

// Represents all Make arguments options
struct Options {
    char** files;

    // Display options
    size_t block_size;
    bool human_readable;
    bool show_total;
    bool show_regular_files; // Show every file, not only just the default of folders
    size_t min_display_size; // Only display files of a certain size
    double min_display_size_percent; // Only display files of a certain percentage of total
    int max_depth; // Only display up to a certain depth

    // Search options
    size_t thread_count;
    bool dereference_symlinks; // Dereference all symlinks
    bool dereference_only_arg_symlinks; // Dereference only symlinks in arguments
    bool track_modification_time; // Track total
    char* use_cache_location; // Use file cache, NULL otherwise
    char* create_cache_location; // Save cache to file, NULL otherwise
};

/**
 * Parse mdu command line arguments
 * ./mdu [-j thread_count] [-B block_size] <file> [files ...] 
 *
 * @return the parsed arguments as an Options struct
 */
Options parse_arguments(int argc, char* argv[]);

/**
 * Try parsing a minimum size str, ex 1000 or 1GB or 50%
 * @param min_size minimum size str
 * @param min_size_bytes output in bytes 
 * @param min_size_percent output in percentage
 * @return true if parsing successful, false otherwise
 * 
 * Valid inputs:
 *      Number with no suffix or following suffix:
 *          K, KB, KiB, M, MB, MiB, G, GB, GiB, T, TB, TiB, %
 *      Suffix is case insensitive
 */
bool try_parse_min_size_str(char* min_size, size_t* min_size_bytes,
                            double* min_size_percent);