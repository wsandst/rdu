# Test script which runs du and rdu on various different folders
# and compares the output
# syntax: ./test.sh <file1> <file2 ...>

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
CLEAR='\033[0m'
failed_test=false

echo "[TEST] Running du and rdu comparison tests..."

for path in $@
do
    rdu_result=`build/debug/rdu -j 2 -B1 $path | head -n1 | awk '{print $1;}'`
    du_result=`du -s -l -B1 $path | head -n1 | awk '{print $1;}'`
    if [ "$du_result" -eq "$rdu_result" ]; then
        echo -e "[TEST] '${path}': ${GREEN} OK ${CLEAR}"
    else
        echo -e "[TEST] '${path}': ${RED} FAIL${CLEAR}"
        echo "du: ${du_result}, rdu: ${rdu_result}"
        failed_test=true
    fi
done

if [ "$failed_test" = true ] ; then
    echo -e "[TEST] ${RED}Comparison tests failed. ${CLEAR}"
    exit 1
fi
echo -e "[TEST] ${GREEN}All comparison tests passed!${CLEAR}"
