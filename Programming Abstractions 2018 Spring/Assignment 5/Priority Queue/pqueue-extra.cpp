/*************************************************************
 * File: pqueue-extra.cpp
 *
 * Implementation file for the ExtraPriorityQueue class.  You
 * do not need to implement this class, but we're happy to
 * give you extra credit if you do!
 */
 
#include "pqueue-extra.h"
#include "error.h"

//initializes values
ExtraPriorityQueue::ExtraPriorityQueue() {
	logLen = 0;
	minIndex = 0;
	min = NULL;
}

//frees alloc space
ExtraPriorityQueue::~ExtraPriorityQueue() {
	for (int i=0; i<roots.size(); i++) {
		cleanUp(roots[i]);
	}
}

//frees allocated space recursively, cleaning vectors of nodes
void ExtraPriorityQueue::cleanUp(nodeT* node) {
	if(node == NULL) return;
	for(int i=0; i<node->children.size(); i++) {
		cleanUp(node->children[i]);
	}
	delete node;
}

//returns size
int ExtraPriorityQueue::size() {
	return logLen;
}

//checks if there are no nodes
bool ExtraPriorityQueue::isEmpty() {
	return size() == 0;
}

//merges two of the selected nodes
void ExtraPriorityQueue::mergeNodes(nodeT* first,nodeT* second,int firstIndex) {
	int largerIndex = 0;
	nodeT* small = NULL;
	nodeT* large = NULL;
	//we need to determine which is a smaller node, since larger becomes a child of smaller
	if (first->value > second->value) {
		largerIndex = firstIndex;
		large = first;
		small = second;
	} else {
		largerIndex = firstIndex-1;
		large = second;
		small = first;
	}
	roots.remove(largerIndex);
	small->children += large;
}

//iterates all over the vector, if there are any nodes with same degree left
void ExtraPriorityQueue::merge() {
	int i = roots.size()-1;
	while(i>0) {
		if(roots[i]->children.size() == roots[i-1]->children.size()) {
			mergeNodes(roots[i],roots[i-1],i);
			i = roots.size()-1; //we reset the index to start iterating again
		} else i--;
	}
}

void ExtraPriorityQueue::enqueue(string value) {
	//creates a new node and adds it to vector
	nodeT* current = new nodeT;
	current->value = value;
	Vector<nodeT*> curVec;
	current->children = curVec;
	roots += current;

	if(isEmpty())  {
		min = current;
		minIndex = 0;
	} else {
		//adds a value to the end of the vector
		if (min->value > current->value) {
			min = current;
			minIndex = roots.size()-1;
		}
	}
	logLen++;
}

//returns min element
string ExtraPriorityQueue::peek() {
	if(isEmpty()) error("Can not peek from an empty queue");
	return min->value;
}

//when a node gets deleted, its children must go up level
void ExtraPriorityQueue::promoteNodes(int index) {
	nodeT* parent = roots.get(index);
	for (int i=0; i<parent->children.size(); i++) {
		//adds every child to vector
		roots += parent->children[i];
	}
	delete roots[index];
}

//returns min element
string ExtraPriorityQueue::dequeueMin() {
	if(isEmpty()) error("Can not dequeue from an empty queue");

	logLen--;
	string minimalVal = min->value;
	
	if(min->children.size()>0) promoteNodes(minIndex);
	roots.remove(minIndex);
	merge();
	
	if(isEmpty()) return minimalVal;

	//since we delete the removed element, we must find the next smallest one
	min = roots[0];
	minIndex = 0;
	for (int i=1; i<roots.size(); i++) {
		if(roots[i]->value < min->value) {
			min = roots[i];
			minIndex = i;
		}
	}
	return minimalVal;
}