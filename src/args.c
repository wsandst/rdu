/**
 * This file implements functionality to parse the command line
 * arguments of the minimal du program
 *
 * @file args.h
 * @author William Sandström
 */
#include "args.h"

/**
 * Parse mdu command line arguments
 *
 * @return the parsed arguments as an Options struct
 */
Options parse_arguments(int argc, char* argv[]) {
    Options options;
    options.files = NULL;
    options.thread_count = 1;
    options.block_size = 512;
    opterr = 0;

    // Parse flags
    int arg_c = getopt(argc, argv, "j:B:");
    while (arg_c != -1) {
        switch (arg_c) {
            case 'j':
                options.thread_count = checked_unsigned_atoi(
                    optarg, "Invalid thread count, must be integer over 0");
                break;
            case 'B':
                options.block_size = checked_unsigned_atoi(
                    optarg, "Invalid block size, must be integer over 0");
                break;
            case '?':
                if (optopt == 'j') {
                    fprintf(stderr, "Option '-%c' requires an argument\n", optopt);
                }
                else {
                    fprintf(stderr, "Unknown option '-%c' provided\n", optopt);
                }
                exit(EXIT_FAILURE);
            default:
                exit(EXIT_FAILURE);
        }
        arg_c = getopt(argc, argv, "j:B:");
    }

    // Parse required and optional arguments
    if (optind < argc) {
        int length = argc - optind;
        // Null terminate using calloc
        options.files = checked_calloc(length + 1, sizeof(char*));
        if (options.files == NULL) {
            perror_and_exit("Argument parsing calloc");
        }
        for (int i = 0; i < length; i++) {
            options.files[i] = argv[i + optind];
        }
    }
    // No files specified, get the working directory instead
    if (options.files == NULL) {
        // Null terminate using calloc
        options.files = checked_calloc(2, sizeof(char*));
        options.files[0] = "."; // Working directory
    }

    return options;
}
