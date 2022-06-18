#!/usr/bin/env bash
# Timing script which runs the supplied command a certain number of times,
# and returns the average time based on several runs
# A single warm-up run is made at the start and not counted
# The amount of times the command runs can be specified with [--count count]
# The script can be made silent with [-s]
GREEN='\033[0;32m'
CLEAR='\033[0m'

# Setup time command settings
TIMEFORMAT=%R                       # 'time' outputs a single number
export LC_NUMERIC="en_US.UTF-8"     # 'time' outputs 0.1 instead of local format, like 0,1

silent=false
durations=()
times=10

# flag options:
#   -silent/-s: run in silent mode, only output average
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -s|--silent) silent=true ;;
        -c|--count) times=$2; shift ;;
        *) break ;;
    esac
    shift
done

output_stream="&1"

if [ "$silent" = true ] ; then
    # Silent option
    $@ > /dev/null

    for i in $(seq $times); 
    do
        duration=`(time $@ > /dev/null 2> /dev/null) 2>&1`
        durations+=("${duration}")
    done

    total=0
    for duration in "${durations[@]}"
    do
        total=`(bc <<< "$total + $duration")`
    done
    average=`bc <<< "scale=4;$total/$times"`
    echo "${average}"
else
    # Non-silent option
    echo "[TEST] Running performance test"
    echo "[TEST] Using command '$@'"
    echo "[TEST] Running warmup..."
    $@ > /dev/null

    echo "[TEST] Timing the command ${times} times"

    for i in $(seq $times); 
    do
        duration=`(time $@ > /dev/null 2> /dev/null) 2>&1`
        echo "[TEST] Run $i: ${duration}s"
        durations+=("${duration}")
    done

    total=0
    for duration in "${durations[@]}"
    do
        total=`(bc <<< "$total + $duration")`
    done

    average=`bc <<< "scale=4;$total/$times"`
    echo -e "[TEST] Average time:$GREEN ${average}$CLEAR sec"
fi
