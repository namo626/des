#include <stdlib.h>
#include <stdio.h>
#include "PrioQ.h"
#include "List.h"

typedef struct PrioQ {
  List* list;
  unsigned int count;
} PrioQ;


PrioQ* PQ_create() {
  PrioQ* new = malloc(sizeof(PrioQ));

  if (new == NULL) {
    printf("Priority Queue creation failed");
    return NULL;
  }

  new->list = list_create();
  new->count = 0;

  return new;
}

unsigned int PQ_count(PrioQ* PQ) {
  return PQ->count;
}

int PQ_insert(PrioQ* PQ, double key, void* data) {
  List* ls = PQ->list;
  int result = list_insert(&ls, key, data);

  // list insertion fails
  if (result == 1) {
    return 1;
  }

  PQ->list = ls;
  PQ->count++;
  return result;
}

void* PQ_delete(PrioQ* PQ, double* key) {
  List* ls = PQ->list;
  if (ls == NULL) {
    printf("Queue is empty\n");
    return NULL;
  }

  double oldKey = ls->listKey;
  void* oldVal = list_delete(&ls);
  PQ->list = ls;
  PQ->count--;

  // fill the key holder
  *key = oldKey;

  return oldVal;
}

void* PQ_free(PrioQ* PQ) {
  List* ls = PQ->list;
  list_free(&ls);
  // ls is now NULL
  PQ->list = ls;
  PQ->count = 0;
  return NULL;
}
