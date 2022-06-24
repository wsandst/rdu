#include <stdio.h>

#include "stack_test.h"

int main() {
    printf("[UNIT-TEST] Running all unit tests...\n");

    test_stack();

    printf("[UNIT-TEST] Passed all unit tests!\n");
    return 0;
}