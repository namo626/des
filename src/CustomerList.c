#include <stdlib.h>
#include "Internal.h"
// a simple linked list (no key) used to hold customers

typedef struct CustNode CustNode;
struct CustNode {
	Customer* cust;
	CustNode* nextCust;
};

struct CustList {
	int amount;
	CustNode* firstCust;
};

int custCount(CustList* clist) {
	return clist->amount;
}

CustNode* mkCustNode(Customer* cust) {
	CustNode* new = malloc(sizeof(CustNode));
	new->cust = cust;
	new->nextCust = NULL;
}

CustList* mkCustList() {
	CustList* new = malloc(sizeof(CustList));
	new->amount = 0;
	new->firstCust = NULL;
}

void freeCustList(CustList* clist) {
	CustNode* head = clist->firstCust;
	CustNode* tmp;

	while (head != NULL) {
		tmp = head;
		head = head->nextCust;
		freeCustomer(tmp->cust);
		if (tmp != NULL) {
			free(tmp);
		}
	}
	if (clist != NULL) {
		free(clist);
	}
}

void insertCust(CustList* clist, Customer* cust) {
	CustNode* new = mkCustNode(cust);
	new->nextCust = clist->firstCust;
	clist->firstCust = new;
	clist->amount++;
}

double maxWaitTime(CustList* clist) {
	CustNode* head = clist->firstCust;

	// list is empty
	if (head == NULL) {
		return 0.0;
	}

	double max = totalWait(head->cust);
	CustNode* current = head->nextCust;
	while (current != NULL) {
		if (totalWait(current->cust) > max) {
			max = totalWait(current->cust);
		}
		current = current->nextCust;
	}

	return max;
}

double minWaitTime(CustList* clist) {
	CustNode* head = clist->firstCust;

	// list is empty
	if (head == NULL) {
		return 0.0;
	}

	double min = totalWait(head->cust);
	CustNode* current = head->nextCust;
	while (current != NULL) {
		if (totalWait(current->cust) < min) {
			min = totalWait(current->cust);
		}
		current = current->nextCust;
	}

	return min;
}

double avgWaitTime(CustList* clist) {
	CustNode* head = clist->firstCust;

	// list is empty
	if (head == NULL) {
		return 0.0;
	}

	double sum = 0;
	CustNode* current = head;
	while (current != NULL) {
		sum = sum + totalWait(current->cust);
		current = current->nextCust;
	}

	return sum / (clist->amount);
}
