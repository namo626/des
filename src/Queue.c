#include <stdlib.h>
#include <stdio.h>
#include "Queue.h"

#define TRUE 0
#define FALSE 1

typedef struct QElem QElem;
struct QElem {
  void* elem;
  QElem* nextElem;
};

struct Queue {
  QElem* first;
  QElem* last;
};

int isEmptyQ(Queue* Q) {
  if (Q->first == NULL && Q->last == NULL) {
    return TRUE;
  }
  return FALSE;
}

QElem* mkQElem(void* E) {
  QElem* new = malloc(sizeof(QElem));
  if (new == NULL) {
    return NULL;
  }

  new->elem = E;
  new->nextElem = NULL;
  return new;
}

Queue* mkQueue() {
  Queue* Q = malloc(sizeof(Queue));
  Q->first = NULL;
  Q->last = NULL;
  return Q;
}

int enqueue(Queue* Q, void* E) {
  QElem* new = mkQElem(E);
  if (new == NULL) {
    printf("Cannot enqueue new element");
    return 1;
  }
  if (isEmptyQ(Q) == TRUE) {
    Q->first = new;
    Q->last = new;
    return 0;
  }

  (Q->last)->nextElem = new;
  Q->last = new;
  return 0;
}

void* dequeue(Queue* Q) {
  if (isEmptyQ(Q) == TRUE) {
    printf("Queue is empty\n");
    return -1;
  }
  QElem* temp = Q->first;
  Q->first = Q->first->nextElem;

  if (Q->first == NULL) {
    Q->last = NULL;
    //printf("Q last is now NULL");
  }
  return temp->elem;
}

/* void printQueue(Queue* Q) { */
/*   QElem* current = Q->first; */
/*   int E; */
/*   while (current != NULL) { */
/*     E = current->elem; */
/*     printf("%d ", E); */
/*     current = current->nextElem; */
/*   } */
/*   printf("\n"); */
/* } */

/* int main() { */
/*   Queue* Q = mkQueue(); */
/*   int x = 10; */
/*   int y = 34; */
/*   int z = 25; */

/*   enqueue(Q, &x); */
/*   printQueue(Q); */
/*   enqueue(Q, &y); */
/*   printQueue(Q); */
/*   enqueue(Q, &z); */
/*   printQueue(Q); */

/*   while (isEmptyQ(Q) == FALSE) { */
/*     dequeue(Q); */
/*     printQueue(Q); */
/*   } */

/*   printQueue(Q); */

/*   return 0; */
/* } */
