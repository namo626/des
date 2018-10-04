#include "List.h"

typedef struct PrioQ PrioQ;

/*
 * Create an empty priority queue
 *
 * @return pointer to the priority queue
 */
PrioQ* PQ_create();

/*
 * Insert a pointer to a data element and its assigned priority to the queue
 *
 * @param PQ pointer to the queue
 * @param key priority of the data
 * @param data pointer to the data element
 * @return 0 if successful, 1 otherwise
 */
int PQ_insert(PrioQ* PQ, double key, void* data);

/*
 * Delete the element in the queue with the lowest priority
 *
 * @param PQ pointer to the queue
 * @return pointer to the deleted element
 */
void* PQ_delete(PrioQ* PQ);

/*
 * Return the number of elements in the queue
 *
 * @param PQ pointer to the queue
 * @return number of elements
 */
unsigned int PQ_count(PrioQ* PQ);

/*
 * Release the memory allocated for the content of the queue
 *
 * @param PQ pointer to the queue
 * @return NULL
 */
void* PQ_free(PrioQ* PQ);
