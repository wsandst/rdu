/**
 * Program to run a minimal du with optional multithreading support.
 *
 * @file rdu.c
 * @author William Sandström
 */

/*
Todo:
    Cleanup code and add proper error handling
    Implement more flags from 'du'
    Add support for keeping track of a search tree with optional cache

    Performance ideas:
        Aggregate multiple search directories somehow
        Lockless stack/queue? Harald had a good idea about this

    Tested performance ideas that didn't work out:
        Manually while-loop in combine_strings instead of strlen and memcpy
        Using file descriptors instead of paths:
            Assigning file descriptors is slow (linear search for free space)
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
