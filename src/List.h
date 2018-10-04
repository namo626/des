#ifndef LIST_H
#define LIST_H

typedef struct List List;

/*
 * Implement a generic key-value linked list
 */
struct List {
  double listKey;   // key
  void* listVal;   // pointer to value
  List* cdr;   // pointer to next list
};

/*
 * Create an empty list (NULL)
 *
 * @return pointer to the list
 */
List* list_create();

/*
 * Delete the first element from the list
 *
 * @param ls pointer to the pointer to the list
 * @return pointer to the deleted element
 */
void* list_delete(List** ls);

/*
 * Insert an element and a key to the list
 *
 * @param ls pointer to the pointer to the list
 * @param key key
 * @param data pointer to the element
 * @return 0 if successful, 1 otherwise
 */
int list_insert(List** ls, double key, void* data);

/*
 * Release the memory containing the list structure
 *
 * @param ls pointer to the pointer to the list
 */
void list_free(List** ls);

#endif
