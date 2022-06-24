/**
 * Stack (LIFO) of C Strings
 * Implemented using a dynamic array
 *
 * @file stack.c
 * @author William Sandström
 */
#include "stack.h"

/**
 * Create a new stack
 * 
 * @param initial_size Initial size of underlying array
 * @return stack
 */
Stack stack_new(size_t initial_size) {
    Stack stack;
    stack.size = 0;
    stack.max_size = initial_size;
    stack.elems = checked_malloc(stack.max_size, sizeof(char*));
    return stack;
}

/**
 * Free the memory of a stack
 * User is responsible for possible pushed dynamic strings
 */
void stack_free(Stack* stack) {
    free(stack->elems);
}

/**
 * Is the stack empty? (has no elements)
 */
bool stack_is_empty(Stack* stack) {
    return stack->size == 0;
}

/**
 * Reverse/resize the underlying array of the stack
 */
void stack_reserve(Stack* stack, size_t new_max_size) {
    stack->max_size = new_max_size;
    stack->elems = checked_realloc(stack->elems, stack->max_size, sizeof(char*));
}

/**
 * Push a string to the end of the stack
 */
void stack_push(Stack* stack, char* elem) {
    stack->size++;
    if (stack->size > stack->max_size) {
        stack_reserve(stack, stack->max_size * 2);
    }
    stack->elems[stack->size - 1] = elem;
}

/**
 * Pop a string from the end of the stack
 */
char* stack_pop(Stack* stack) {
    stack->size--;
    return stack->elems[stack->size];
}

void stack_append(Stack* stack1, Stack* stack2) {
    // Make sure stack1 is large enough
    if (stack2->size == 0) {
        return;
    }
    size_t new_size = stack1->size + stack2->size;
    if (new_size > stack1->max_size) {
        stack_reserve(stack1, new_size * 2);
    }

    memcpy(stack1->elems + stack1->size, stack2->elems, stack2->size * sizeof(char*));
    stack1->size = new_size;
}