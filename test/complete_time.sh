#!/usr/bin/env bash
# Script to test the performance of rdu on different amount of threads on /pkg/
# Saves the output to a file
cd $(dirname $0)

# Make sure required arguments are passed
if [ "$#" -lt 3 ]; then
    echo "Usage: ./complete_time.sh <dir> <max_thread_count> <runs_per_thread>"
    exit 1
fi

directory=$1
max_threads=$2
times_per_run=$3

echo "[TEST] Running complete multi-threaded rdu performance test on dir '$directory'"
echo "[TEST] Using up to $max_threads threads"

# Clear file if it already exists
# >$output_file

# Run time.sh script for every thread up to max thread count
for threads in $(seq $max_threads); 
do
    avg_time=`./time.sh -s -c $times_per_run ./../build/release/rdu -j $threads $directory`
    echo "[TEST] rdu with $threads threads took on average: ${avg_time}s"
    # Save avg time to output file
    # echo "$avg_time" >> $output_file
done