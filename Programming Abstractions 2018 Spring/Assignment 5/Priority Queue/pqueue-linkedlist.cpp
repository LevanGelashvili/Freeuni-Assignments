/*************************************************************
 * File: pqueue-linkedlist.cpp
 *
 * Implementation file for the LinkedListPriorityQueue
 * class.
 */
 
#include "pqueue-linkedlist.h"
#include "error.h"

LinkedListPriorityQueue::LinkedListPriorityQueue() {
	first = NULL;
	logLen = 0;
}

LinkedListPriorityQueue::~LinkedListPriorityQueue() {
	//empties allocated space
	nodeT* tmp = NULL;
	while(first!=NULL) {
		tmp = first->next;
		delete first;
		first = tmp;
	}
}	

int LinkedListPriorityQueue::size() {
	return logLen;
}

bool LinkedListPriorityQueue::isEmpty() {
	return size() == 0;
}

void LinkedListPriorityQueue::enqueue(string value) {
	logLen++;

	//creates a new node
	nodeT* current = new nodeT;
	current->word = value;
	current->next = NULL;

	if(first == NULL) {
		first = current;
		return;
	} else if(current->word < first->word) {
		//checks if a node has to go to the first space
		current->next = first;
		first = current;
		return;
	}

	nodeT* tmp = first;
	string minimal = current->word;

	//checks for the minimal element
	while(tmp->next != NULL) {
		if(minimal < tmp->next->word) {
			current->next = tmp->next;
			tmp->next = current;
			return;
		} else {
			tmp = tmp->next;
		}
	}
	//if no element has been smaller, inserts node in the end of a list
	tmp->next = current;
}

string LinkedListPriorityQueue::peek() {
	if(isEmpty()) error("Queue is empty, can not peek");
	return first->word;
}

string LinkedListPriorityQueue::dequeueMin() {
	if(isEmpty()) error("Queue is empty, can not dequeueMin");

	string str = first->word;
	if(first->next != NULL) {
		nodeT* tmp = first->next;
		delete first;
		first = tmp;
	} else {
		//only one element
		first = NULL;
	}
	logLen--;
	return str;
}

