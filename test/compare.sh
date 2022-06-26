#!/usr/bin/env bash
# Comparison test between different du:s
cd $(dirname $0)

# Compare between du, rdu, mdu, Harald mdu, diskus, tin-summer, dust

# Make sure required arguments are passed
if [ "$#" -lt 1 ]; then
    echo "Usage: ./compare.sh <directory>"
    exit 1
fi

dir=$1

du_cmd="du -s -l -B1 /home/williamsandst/"
rdu_cmd="../build/release/rdu -j 16 $dir"
mdu_cmd="$HOME/repos/umu/sysn/sysn-ou3/build/release/mdu -j 16 $dir"
mdu_st_cmd="$HOME/repos/umu/sysn/sysn-ou3/build/release/mdu -j 1 $dir"
mdu_hbf_cmd="$HOME/repos/umu/sysn/mdu-hbf/mdu_competition -j 16 $dir"
mdu_st_hbf_cmd="$HOME/repos/umu/sysn/mdu-hbf/mdu_competition -j 1 $dir"
diskus_cmd="diskus $dir"
tinsummer_cmd="sn p -d0 -j16 $dir"
tinsummer_st_cmd="sn p -d0 -j1 $dir"

hyperfine -i --export-markdown ../build/comp.md --warmup 3 \
    --command-name "du" "$du_cmd" \
    --command-name "mdu st (Harald)" "$mdu_st_hbf_cmd" \
    --command-name "mdu st (William)" "$mdu_st_cmd" \
    --command-name "tin-summer st" "$tinsummer_st_cmd" \
    --command-name "tin-summer mt" "$tinsummer_cmd" \
    --command-name "diskus mt" "$diskus_cmd" \
    --command-name "mdu mt (Harald)" "$mdu_hbf_cmd" \
    --command-name "mdu mt (William)" "$mdu_cmd" \
    --command-name "rdu mt (William)" "$rdu_cmd" 