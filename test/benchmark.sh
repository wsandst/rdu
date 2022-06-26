#!/usr/bin/env bash
# Script to test the performance of rdu on different amount of threads on /pkg/
# Saves the output to a file
cd $(dirname $0)

# Make sure required arguments are passed
if [ "$#" -lt 2 ]; then
    echo "Usage: ./complete_time.sh <max_thread_count> <dir>"
    exit 1
fi

directory=$2
max_threads=$1

echo "[TEST] Running complete multi-threaded rdu performance test on dir '$directory'"
echo "[TEST] Using up to $max_threads threads"

# Clear file if it already exists
# >$output_file

hyperfine --warmup 2 --export-markdown ../build/bench.md --parameter-scan num_threads 1 $max_threads "./../build/release/rdu -j {num_threads} $directory" 