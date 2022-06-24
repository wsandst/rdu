#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../../src/util/stack.h"

void test_stack_push_pop();
void test_stack_append();

void test_stack() {
    printf("[UNIT-TEST] Running stack tests...\n");

    test_stack_push_pop();
    test_stack_append();

    printf("[UNIT-TEST] Passed stack tests!\n");
}

void test_stack_push_pop() {
    // Test simple pushing and popping
    Stack stack = stack_new(2);
    assert(stack_is_empty(&stack));
    assert(stack.size == 0);
    assert(stack.max_size == 2);
    stack_push(&stack, "hello world");
    assert(stack.size == 1);

    char* top = stack_pop(&stack);
    assert(strcmp(top, "hello world") == 0);

    stack_push(&stack, "test1");
    stack_push(&stack, "test2");
    assert(stack.size == 2);
    assert(stack.max_size == 2);
    stack_push(&stack, "test3");
    assert(stack.size == 3);
    assert(stack.max_size == 4);

    assert(strcmp("test3", stack_pop(&stack)) == 0);
    assert(strcmp("test2", stack_pop(&stack)) == 0);
    assert(strcmp("test1", stack_pop(&stack)) == 0);
    assert(stack.size == 0);
    assert(stack.max_size == 4);

    stack_free(&stack);
}

void test_stack_append() {
    // Test appending 2 stacks
    Stack stack1 = stack_new(2);
    Stack stack2 = stack_new(2);

    stack_push(&stack1, "test1");
    stack_push(&stack1, "test2");
    stack_push(&stack1, "test3");
    assert(stack1.size == 3);

    stack_push(&stack2, "test4");
    stack_push(&stack2, "test5");
    stack_push(&stack2, "test6");
    stack_push(&stack2, "test7");
    assert(stack2.size == 4);

    stack_append(&stack1, &stack2);
    assert(stack1.size == 7);
    assert(stack2.size == 4);

    assert(strcmp("test7", stack_pop(&stack1)) == 0);
    assert(strcmp("test6", stack_pop(&stack1)) == 0);
    assert(strcmp("test5", stack_pop(&stack1)) == 0);
    assert(strcmp("test4", stack_pop(&stack1)) == 0);
    assert(strcmp("test3", stack_pop(&stack1)) == 0);
    assert(strcmp("test2", stack_pop(&stack1)) == 0);
    assert(strcmp("test1", stack_pop(&stack1)) == 0);
    assert(stack_is_empty(&stack1));

    Stack stack3 = stack_new(2);
    stack_append(&stack1, &stack3);
    assert(stack1.size == 0);
    stack_push(&stack3, "test1");
    stack_append(&stack1, &stack3);
    assert(stack1.size == 1);
    assert(strcmp("test1", stack_pop(&stack1)) == 0);

    stack_free(&stack1);
    stack_free(&stack2);
    stack_free(&stack3);
}