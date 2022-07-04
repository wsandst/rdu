/**
 * This file implements functionality to parse the command line
 * arguments of the minimal du program
 *
 * @file args.h
 * @author William Sandström
 */
#include "args.h"

static int arg_summarize = 0;
static int arg_human_readable = 0;
static int arg_show_every_file = 0;
static int arg_track_modification_time = 0;
static int arg_show_total = 0;
static int arg_dereference_symlinks = 0;
static int arg_dereference_only_arg_symlinks = 0;

/**
 * Parse mdu command line arguments
 *
 * @return the parsed arguments as an Options struct
 */
Options parse_arguments(int argc, char* argv[]) {
    Options options = { 0 };
    options.min_display_size_percent = 0;
    options.files = NULL;
    options.thread_count = sysconf(_SC_NPROCESSORS_ONLN);
    options.block_size = 1024;
    opterr = 0;

    int option_index = 0;
    struct option long_options[] = {
        { "human-readable", no_argument, &arg_summarize, 'h' },
        { "summarize", no_argument, &arg_summarize, 's' },
        { "all", no_argument, &arg_show_every_file, 'a' },
        { "time", no_argument, &arg_track_modification_time, 'T' },
        { "total", no_argument, &arg_show_total, 'c' },
        { "max-depth", required_argument, 0, 'd' },
        { "dereference", no_argument, &arg_dereference_symlinks, 'L' },
        { "dereference-args", no_argument, &arg_dereference_only_arg_symlinks, 'D' },
        { "block-size", required_argument, 0, 'B' },
        { "threads", required_argument, 0, 'j' },
        { "create-cache", optional_argument, 0, 'C' },
        { "use-cache", optional_argument, 0, 'u' },
        { "threshold", required_argument, 0, 't' },
        { 0, 0, 0, 0 }
    };

    // Parse flags
    int arg_c = getopt_long(argc, argv, ":hsaTcd:LDB:j:C::u::t:", long_options,
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
            case 't':
                // Min size (in bytes or percent)
                if (!try_parse_min_size_str(optarg, &options.min_display_size,
                                            &options.min_display_size_percent)) {
                    stderr_and_exit(
                        "Invalid threshold size option, must be valid number (below 2^64) or percentage (0-100%)");
                }
                break;
            case 'h':
                arg_human_readable = true;
                break;
            case 's':
                arg_summarize = true;
                break;
            case 'a':
                arg_show_every_file = true;
                break;
            case 'T':
                arg_track_modification_time = true;
                break;
            case 'c':
                arg_show_total = true;
                break;
            case 'L':
                arg_dereference_symlinks = true;
                break;
            case 'D':
                arg_dereference_only_arg_symlinks = true;
                break;
            case '?':
                fprintf(stderr, "rdu: Invalid option '-%c' provided\n", optopt);
                exit(EXIT_FAILURE);
            case ':':

                fprintf(stderr, "rdu: Option '-%c' requires an argument\n", optopt);
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

    if (arg_summarize && arg_show_every_file) {
        stderr_and_exit("Cannot both summarize and show all entries");
    }
    if (arg_summarize) {
        options.max_depth = 0;
    }
    else {
        options.max_depth = -1;
    }
    options.human_readable = arg_human_readable;
    options.show_regular_files = arg_show_every_file;
    options.track_modification_time = arg_track_modification_time;
    options.show_total = arg_show_total;
    options.dereference_symlinks = arg_dereference_symlinks;
    options.dereference_only_arg_symlinks = arg_dereference_only_arg_symlinks;

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
 *          K, KB, KiB, M, MB, MiB, G, GB, GiB, T, TB, TiB, B, %
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
    bool is_power_of_two_factor = true;
    char suffix = input_num_end[0];
    switch (suffix) {
        case 'k':
        case 'K':
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
        case 'b':
        case 'B':
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
                // B suffix, this is a power of 10
                is_power_of_two_factor = false;
                break;
            case '\0':
            case 'i':
            case 'I':
                // Kibi/mibi byte etc, this is a power of 2
                break;
            default:
                // Invalid suffix character
                return false;
        }
    }

    if (is_percentage) {
        if (input_num > 100 || input_num < 0) {
            return false;
        }
        *min_size_percent = input_num / 100;
    }
    else {
        double factor = is_power_of_two_factor ? pow(1024, exp_factor) :
                                                 pow(1000, exp_factor);
        double result = input_num * factor;
        if (result < 1 || !isnormal(result) || result > ULLONG_MAX) {
            // Non-integer bytes or bad float, fail
            return false;
        }
        *min_size_bytes = input_num * factor;
    }

    return true;
}