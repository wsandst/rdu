/**
 * Program to run a minimal du with optional multithreading support.
 *
 * @file mdu.c
 * @author William Sandström (c20wsm)
 * @date ADD ON HANDIN
 */

/*

Todo:
    Cleanup multithreaded code:
        Maybe change pop behaviour so that we return NULL if zero? Would
        reduce the function calls needed
    Improve performance of multithreaded code:
        What happens if we only push directories to the pool?
        We do a malloc for every directory currently, can we avoid this?
        We currently push a ton and then pop/push, we could just memcpy here
    Implement

    Performance ideas:
        Test if breadth-first multiple directories work better
        Need some solid way of benchmarking, itchy is inconsistent
        Benchmark code
        Sort directories files by inode number? Is this relevant on an SSD?
            I could 

    Tested performance ideas that didn't work out:
        Manually while-loop in combine_strings instead of strlen and memcpy
        Multiple directories per thread (depth first)
        Using file descriptors instead of paths:
            Faster single-threaded, but slower with many threads
            Why?

    Handin: 
        Make sure all the requirements are passed
        Work on report
        Create a separate variant for performance maybe?

*/

#include <stdio.h>

#include "args.h"
#include "disk_usage.h"

int main(int argc, char* argv[]) {
    Options options = parse_arguments(argc, argv);

    disk_usage(options);

    free(options.files);

    return 0;
}
