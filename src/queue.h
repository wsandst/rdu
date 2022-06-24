/**
 * Queue (FIFO) of C Strings
 * Implemented using a circular dynamic array
 *
 * @file queue.h
 * @author William Sandström
 */

#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/helpers.h"

struct Queue {
    size_t size;
    int front_index; // Front/top of queue
    int back_index; // Back/end of queue

    size_t max_size;
    char** elems_start;
    char** elems_end;
};

typedef struct Queue Queue;

/**
 * Create a new queue
 * 
 * @param initial_size Initial size of underlying array
 * @return queue
 */
Queue queue_new(size_t initial_size);

/**
 * Free the memory of a queue
 * User is responsible for possible pushed dynamic strings
 */
void queue_free(Queue* queue);

/**
 * Reverse/resize the underlying array of the queue
 */
void queue_reserve(Queue* queue, size_t new_max_size);

/**
 * Is the queue empty? (has no elements)
 */
bool queue_is_empty(Queue* queue);

/**
 * Enqueue a string to the end of the queue
 */
void queue_enqueue(Queue* queue, char* elem);

/**
 * Dequeue a string from the front of the queue
 */
char* queue_dequeue(Queue* queue);