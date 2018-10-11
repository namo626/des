#ifndef QUEUE_H
#define QUEUE_H

/*
 * This is a library for an integer FIFO queue
 */

typedef struct Queue Queue;

/*
 * Create an empty queue
 *
 * @return pointer to the queue
 */
Queue* mkQueue();

void* viewHead(Queue* Q);

/*
 * Add an element to the queue
 *
 * @param Q the queue
 * @param E integer element
 * @return 0 if successful, 1 otherwise
 */
int enqueue(Queue* Q, void* E);

/*
 * Remove an element from the queue
 *
 * @param Q the queue
 * @return the removed element (integer value)
 */
void* dequeue(Queue* Q);


/*
 * Check if the queue is empty
 *
 * @param Q the queue
 * @return 0 if true, 1 otherwise
 */
int isEmptyQ(Queue* Q);

#endif
