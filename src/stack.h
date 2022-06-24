/**
 * Stack (LIFO) of C Strings
 * Implemented using a dynamic array
 *
 * @file stack.h
 * @author William Sandström
 */

#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/helpers.h"

struct Stack {
    size_t size;
    size_t max_size;
    char** elems;
};

typedef struct Stack Stack;

/**
 * Create a new stack
 * 
 * @param initial_size Initial size of underlying array
 * @return stack
 */
Stack stack_new(size_t initial_size);

/**
 * Free the memory of a stack
 * User is responsible for possible pushed dynamic strings
 */
void stack_free(Stack* stack);

/**
 * Reverse/resize the underlying array of the stack
 */
void stack_reserve(Stack* stack, size_t new_max_size);

/**
 * Is the stack empty? (has no elements)
 */
bool stack_is_empty(Stack* stack);

/**
 * Push a string to the end of the stack
 */
void stack_push(Stack* stack, char* elem);

/**
 * Pop a string from the end of the stack
 */
char* stack_pop(Stack* stack);

/**
 * Add stack2 to the end of stack1 using memcpy
 */
void stack_append(Stack* stack1, Stack* stack2);