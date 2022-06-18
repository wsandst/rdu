/**
 * Queue (FIFO) of C Strings
 * Implemented using a circular dynamic array
 * 
 * [ . . . FRONT X X X X X X BACK . . . ]
 *
 * @file queue.h
 * @author William Sandström
 */
#include "queue.h"

/**
 * Create a new queue
 * 
 * @param initial_size Initial size of underlying array
 * @return queue
 */
Queue queue_new(size_t initial_size) {
    Queue queue;
    queue.size = 0;
    queue.max_size = initial_size;
    queue.elems_start = checked_malloc(queue.max_size, sizeof(char*));
    queue.elems_end = queue.elems_start + queue.max_size;
    queue.front_index = 0;
    queue.back_index = 0;
    return queue;
}

/**
 * Free the memory of a queue
 * User is responsible for possible pushed dynamic strings
 */
void queue_free(Queue* queue) {
    free(queue->elems_start);
}

/**
 * Reverse/resize the underlying array of the queue
 */
void queue_reserve(Queue* queue, size_t new_max_size) {
    // Make array continous, which allows for realloc
    int right_section_length = queue->max_size - queue->front_index;
    queue->max_size = new_max_size;
    queue->elems_start = checked_realloc(queue->elems_start, new_max_size, sizeof(char*));
    queue->elems_end = queue->elems_start + new_max_size;
    if (queue->front_index > queue->back_index) {
        // Front is after back, we need to move it so the entire region is continuous
        memcpy(queue->elems_start + queue->front_index + right_section_length,
               queue->elems_start, queue->back_index * sizeof(char*));
        queue->back_index = queue->front_index + right_section_length + queue->back_index;
    }
}

/**
 * Is the queue empty? (has no elements)
 */
bool queue_is_empty(Queue* queue) {
    return queue->size == 0;
}

/**
 * Enqueue a string to the end of the queue
 */
void queue_enqueue(Queue* queue, char* elem) {
    queue->size++;
    if (queue->size > (queue->max_size - 1)) {
        queue_reserve(queue, queue->max_size * 2);
    }
    queue->elems_start[queue->back_index] = elem;
    queue->back_index = (queue->back_index + 1) % queue->max_size;
}

/**
 * Dequeue a string from the front of the queue
 */
char* queue_dequeue(Queue* queue) {
    char* elem = queue->elems_start[queue->front_index];
    queue->size--;
    queue->front_index = (queue->front_index + 1) % queue->max_size;
    return elem;
}