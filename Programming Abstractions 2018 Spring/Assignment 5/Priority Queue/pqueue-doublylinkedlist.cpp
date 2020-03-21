/*************************************************************
 * File: pqueue-doublylinkedlist.cpp
 *
 * Implementation file for the DoublyLinkedListPriorityQueue
 * class.
 */
 
#include "pqueue-doublylinkedlist.h"
#include "error.h"

DoublyLinkedListPriorityQueue::DoublyLinkedListPriorityQueue() {
	first = NULL;
	logLen = 0;
}

DoublyLinkedListPriorityQueue::~DoublyLinkedListPriorityQueue() {
	//frees allocated space
	nodeT* tmp = NULL;
	while(first!=NULL) {
		tmp = first->next;
		delete first;
		first = tmp;
	}
}

int DoublyLinkedListPriorityQueue::size() {
	return logLen;
}

bool DoublyLinkedListPriorityQueue::isEmpty() {
	return size() == 0;
}

void DoublyLinkedListPriorityQueue::enqueue(string value) {
	//creates a node
	nodeT* cur = new nodeT;
	cur->next = NULL;
	cur->last = NULL;
	cur->word = value;

	if(isEmpty()) {
		first = cur;
	} else {
		//inserts a node in the first space
		first->last = cur;
		cur->next = first;
		first = cur;
	}
	logLen++;
}

string DoublyLinkedListPriorityQueue::peek() {
	if(isEmpty()) error("Can not peek from an empty queue");

	//iterates over the list and chooses minimal element
	string minimal = first->word;
	for (nodeT* tmp = first; tmp!= NULL; tmp = tmp->next){
		if(tmp->word < minimal) minimal = tmp->word;
	}
	return minimal;
}

string DoublyLinkedListPriorityQueue::dequeueMin() {
	if(isEmpty()) error("Can not dequeue from an empty queue");

	//iterates over the list and chooses minimal element, to remove
	string minimal = first->word;
	nodeT* minNode = NULL;
	for (nodeT* tmp = first; tmp!= NULL; tmp = tmp->next){
		if(tmp->word <= minimal) {
			minimal = tmp->word;
			minNode = tmp;
		}
	}
	
	//checks special cases, in order not to mess up the list
	if(minNode->last == NULL) {
		//checks if element is first
		if(minNode->next != NULL) {
			first = minNode->next;
			first->last = NULL;
		} else {
			first = NULL;
		}
		//checks if eleent is last
	} else if(minNode->next == NULL) {
		minNode->last->next = NULL;
	} else {
		minNode->last->next = minNode->next;
		minNode->next->last = minNode->last;
	}
	logLen--;
	delete minNode;
	return minimal;
}

