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
        -h , --human-readable (end action)
        -s, --summarize: This is how it works currently by default, summarize for passed folder (thread action)
        -a, --all: Show every file and folder (end action, thread action (keep tree))
        -t, --time: Include the last modification time (end action, thread action (keep tree))
        -c, --total: Show total at the end under 'total', same as summarize (end action)
        -d, --max-depth=N: Max depth to print size of, max-depth=0 is same as summarize (thread action, keep ply in tree)
        -L: dereference symbolic links (thread action)
        -P: do not dereference symbolic links (default) (thread action)
        -D, --dereference-args, also -H: deference only symbolic links sent directly in command-line as folders to check (start action)
        -B, --block-size=SIZE: scale sizes by SIZE before printing them (end action, already have this)
    My own flags:
        -j, --threads: max amount of threads to use. Default is to determine dynamically (start action, already have this)
        -C, --use-cache: use and save to a cache file. This cache file will be used to speed up (thread action, use tree, then end action, save tree)
            the next searches. Can optionally specify file location here, will use a tmp/ otherwise
        -m, --min-size: The minimum size of a folder to display. This can be in plain bytes, human readable or percentage. (end action, keep tree)
    
    Add support for keeping track of a search tree with optional cache:
        

    Alright, in the threads, we have two main options:
        Do not keep tree (depth=0 only)
        Keep tree:
            If we keep tree, we need to keep track of time information.
            Every node also needs the name and the size. 
            If cache save is enabled, we save the tree at the end
            If cache usage is enabled, at the start we traverse the thread single-threaded
            and check for a match. If no match, search normally
            Is cache save and usage fine in same? I guess, just need to start a new root node

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
