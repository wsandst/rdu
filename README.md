![Testing status](https://github.com/wsandst/rdu/actions/workflows/ci.yml/badge.svg?branch=main)
# Rapid Disk Usage
`rdu` is a command line utility for determining disk usage of folders and files recursively, designed for maximal speed. 
The tool is very similar to the built-in linux utility `du`. By utilizing threads and better system calls, the performance has been increased drasticly.

# Benchmarking

# Command-line arguments
`rdu` supports most of the commands from `du`. Some additional commands have been added, for example specifying thread count, cache usage or percentage display.

Usage: `rdu [OPTION] [FILE1] [...]`  
Usage example: `rdu /etc/ -hs`

### Supported plain `du` flags
    -h, --human-readable
    -s, --summarize: Only display the size of the passed folders
    -a, --all: Show every file and folder
    -t, --time: Include the last modification time
    -c, --total: Show total at the end under 'total', same as summarize
    -d, --max-depth=N: Max depth to print size of, max-depth=0 is same as summarize
    -L: dereference symbolic links
    -P: do not dereference symbolic links (default)
    -D, --dereference-args, also -H: deference only symbolic links sent directly in command-line as folders to check
    -B, --block-size=SIZE: scale sizes by SIZE before printing them
### Additional added flags
    -T, --threads: max amount of threads to use. Default is to determine dynamically
    -C, --create-cache: Create a new cache file. This will be stored in /tmp/ by default, or in a user specified location
    -u, --use-cache: Use a created file cache. This will greatly speed up retrieving of disk usage, but this assumes that none of the files have changed since the cache creation, otherwise the results will be wrong!
    -p, --percentage: show percentage usage compared to root folder
    -m, --min-size: The minimum size of a folder to display. This can be in plain bytes, human readable or percentage.