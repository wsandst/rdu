#include <stdio.h>

#include "stack_test.h"
#include "file_node_test.h"
#include "arg_parsing_test.h"

int main() {
    printf("[UNIT-TEST] Running all unit tests...\n");

    test_stack();
    test_file_node();
    test_arg_parsing();

    printf("[UNIT-TEST] Passed all unit tests!\n");
    return 0;
}