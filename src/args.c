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
    options.thread_count = sysconf(_SC_NPROCESSORS_ONLN);
    options.block_size = 512;
    opterr = 0;

    int option_index = 0;
    struct option long_options[] = {
        { "human-readable", no_argument, (int*) &options.human_readable, 'h' },
        { "summarize", no_argument, (int*) &options.summarize, 's' },
        { "all", no_argument, (int*) &options.show_every_file, 'a' },
        { "time", no_argument, (int*) &options.track_modification_time, 't' },
        { "total", no_argument, (int*) &options.show_total, 'c' },
        { "max-depth", required_argument, 0, 'd' },
        { "dereference", no_argument, (int*) &options.dereference_symlinks, 'L' },
        { "dereference-args", no_argument, (int*) &options.dereference_only_arg_symlinks,
          'D' },
        { "block-size", required_argument, 0, 'B' },
        { "threads", required_argument, 0, 'j' },
        { "create-cache", optional_argument, 0, 'C' },
        { "use-cache", optional_argument, 0, 'u' },
        { "min-size", required_argument, 0, 'm' },
        { 0, 0, 0, 0 }
    };

    // Parse flags
    int arg_c = getopt_long(argc, argv, ":hsatcd:LDB:j:C::u::m:", long_options,
                            &option_index);
    while (arg_c != -1) {
        switch (arg_c) {
            case 'j':
                // Thread count
                options.thread_count = checked_unsigned_atoi(
                    optarg, "Invalid thread count, must be integer over 0");
                break;
            case 'B':
                // Block size
                options.block_size = checked_unsigned_atoi(
                    optarg, "Invalid block size, must be integer over 0");
                break;
            case 'd':
                // Max depth
                options.max_depth = checked_unsigned_atoi(
                    optarg, "Invalid max depth option, must be integer over 0");
                break;
            case 'C':
                // Create cache
                if (optarg) {
                    options.create_cache_location = optarg;
                }
                else {
                    options.create_cache_location = "/tmp/rdu-tree-cache.dat";
                }
                break;
            case 'u':
                // Use cache
                if (optarg) {
                    options.use_cache_location = optarg;
                }
                else {
                    options.use_cache_location = "/tmp/rdu-tree-cache.dat";
                }
                break;
            case 'm':
                // Min size (in bytes or percent)
                if (!try_parse_min_size_str(optarg, &options.min_display_size,
                                            &options.min_display_size_percent)) {
                    stderr_and_exit("Invalid min size option, must be number or percentage");
                }
                break;
            case '?':
                fprintf(stderr, "Unknown option '-%c' provided\n", optopt);
                exit(EXIT_FAILURE);
            case ':':
                fprintf(stderr, "Option '-%c' requires an argument\n", optopt);
                exit(EXIT_FAILURE);
                break;
        }
        arg_c = getopt_long(argc, argv, "-:hsatcd:LDB:j:C::u::m:", long_options,
                            &option_index);
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
                            double* min_size_percent) {
    char* input_num_end;
    double input_num = strtod(min_size, &input_num_end);

    if (input_num == 0) {
        return false; // Failed conversion
    }

    long exp_factor = 1;
    bool is_percentage = false;
    bool is_power_of_two_factor = false;
    char suffix = input_num_end[0];
    switch (suffix) {
        case 'K':
        case 'k':
            exp_factor = 1; // 10^3 == 1000^1, 2^10 == 1024^1
            break;
        case 'm':
        case 'M':
            exp_factor = 2; // 10^6, 2^20
            break;
        case 'g':
        case 'G':
            exp_factor = 3; // 10^9, 2^30
            break;
        case 't':
        case 'T':
            exp_factor = 4; // 10^12, 2^40
            break;
        case '\0':
            exp_factor = 0;
            break;
        case '%':
            is_percentage = true;
            break;
        default:
            // Invalid suffix character
            return false;
    }
    if (suffix != '\0') {
        // A second suffix character is available
        char suffix2 = input_num_end[1];
        switch (suffix2) {
            case 'b':
            case 'B':
            case '\0':
                // Ok, do nothing
                break;
            case 'i':
            case 'I':
                // Kibi/mibi byte etc, this is a power of 2
                is_power_of_two_factor = true;
                break;
            default:
                // Invalid suffix character
                return false;
        }
    }

    if (is_percentage) {
        if (input_num > 100) {
            return false;
        }
        *min_size_percent = input_num / 100;
    }
    else {
        double factor = is_power_of_two_factor ? pow(1024, exp_factor) :
                                                 pow(1000, exp_factor);
        if ((input_num * factor) < 1) {
            // Non-integer bytes, bad!
            return false;
        }
        *min_size_bytes = input_num * factor;
    }

    return true;
}