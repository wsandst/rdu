# Compare between current release and old mdu, to prevent performance regression

cd $(dirname $0)

if [ "$#" -lt 1 ]; then
    echo "Usage: ./compare.sh <directory>"
    exit 1
fi

dir=$1

rdu_cmd="../build/release/rdu -j 32 $dir"
rdu_st_cmd="../build/release/rdu -j 1 $dir"
mdu_cmd="$HOME/repos/umu/sysn/sysn-ou3/build/release/mdu -j 32 $dir"
mdu_st_cmd="$HOME/repos/umu/sysn/sysn-ou3/build/release/mdu -j 1 $dir"

hyperfine -i --export-markdown ../build/reg-comp.md --warmup 3 \
    --command-name "rdu" "$rdu_cmd" \
    --command-name "rdu st" "$rdu_st_cmd" \
    --command-name "mdu mt" "$mdu_cmd" \
    --command-name "mdu st" "$mdu_st_cmd" \