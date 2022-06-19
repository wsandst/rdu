/**
 * Program to run a minimal du with optional multithreading support.
 *
 * @file rdu.c
 * @author William Sandström
 */

/*
Todo:
    Cleanup code and add proper error handling
    Implement more flags from 'du':
        -h , --human-readable
        -s, --summarize: This is how it works currently by default, summarize for passed folder
        -a, --all: Show every file and folder
        -t, --time: Include the last modification time
        -c, --total: Show total at the end under 'total', same as summarize
        -d, --max-depth=N: Max depth to print size of, max-depth=0 is same as summarize
        -L: dereference symbolic links
        -P: do not dereference symbolic links (default)
        -D, --dereference-args, also -H: deference only symbolic links sent directly in command-line as folders to check
        -B, --block-size=SIZE: scale sizes by SIZE before printing them
    My own flags:
        -T, --threads: max amount of threads to use. Default is to determine dynamically
        -C, --use-cache: use and save to a cache file. This cache file will be used to speed up 
            the next searches. Can optionally specify file location here, will use a tmp/ otherwise
        -p, --percentage: show percentage usage compared to root folder
        -m, --min-size: The minimum size of a folder to display. This can be in plain bytes, human readable or percentage.
    
    Add support for keeping track of a search tree with optional cache:
        Keep a tree of every folder with sizes. Once the cache is created, convert
        this tree into a hashmap. Save this hashmap to a file.
        When using the cache, the naive way is to just check if the folder is in the hashmap.
        I lose linking information by doing this though. One way would be to include the folder
        names with the item. Sounds slow. Might be better to just linearize the tree somehow? 
        Would allow for updating caches as well with new folders and such.

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
