#!/usr/bin/env bash
# Simple script to test the performance of mdu on /pkg/
cd $(dirname $0)

if [ "$#" -lt 1 ]; then
    echo "Please specify the amount of threads to use"
    exit 1
fi

echo "[TEST] Running /pkg/ performance test with $1 threads"

bash time.sh ../build/release/mdu -j $1 /pkg/