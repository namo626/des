#include <stdio.h>
#include <stdlib.h>
#include "List.h"

List* list_create() {
	return NULL;
}

void* list_delete(List** ls) {
	if (*ls == NULL) {
		printf("Error: List is empty");
		return NULL;
	}
	List* old = *ls;
	// make ls point to its cdr
	void* car = (*ls)->listVal;
	*ls = (*ls)->cdr;
	if (old != NULL) {
		free(old);
	}
	return car;
}

/*
 * Insertion is adjusted from the Java implementation
 * from the book "Introduction to Java Programming" by Daniel Liang
 */
int list_insert(List** ls, double key, void* data) {
	// create a new node for the new item
	List* new = malloc(sizeof(List));
	if (new == NULL) {
		printf("Failed to create a new List");
		return 1;
	}

	new->listKey = key;
	new->listVal = data;

	// current and previous nodes for insertion
	List* previous = NULL;
	List* current = *ls;

	while (current != NULL && key > (current->listKey)) {
		previous = current;
		current = current->cdr;
	}

	// current still at first of list
	if (previous == NULL) {
		new->cdr = *ls;
		*ls = new;
	} else {
		previous->cdr = new;
		new->cdr = current;
	}

	return 0;
}

void list_free(List** ls) {
	List* curr = *ls;
	List* temp;
	while (curr != NULL) {
		temp = curr;
		curr = curr->cdr;
		free(temp);
	}

	// avoid dangling pointer
	*ls = NULL;
}
