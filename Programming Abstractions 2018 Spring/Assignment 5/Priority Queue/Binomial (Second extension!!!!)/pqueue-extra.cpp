/*************************************************************
 * File: pqueue-extra.cpp
 *
 * Implementation file for the ExtraPriorityQueue class.  You
 * do not need to implement this class, but we're happy to
 * give you extra credit if you do!
 */
 
#include "pqueue-extra.h"
#include "error.h"

ExtraPriorityQueue::ExtraPriorityQueue() {
	logLen = 0;
}

//frees allocated space
ExtraPriorityQueue::~ExtraPriorityQueue() {
	for (int i=0; i<roots.size(); i++) {
		cleanUp(roots[i]);
	}
}

//deletes nodes in vector
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

//iterates over the vector to choose the nodes to be merged (of same degree)
void ExtraPriorityQueue::merge() {
	int i = roots.size()-1;
	while(i>0) {
		if(roots[i]->children.size() == roots[i-1]->children.size()) {
			mergeNodes(roots[i],roots[i-1],i);
			i = roots.size()-1;
		} else i--;
	}
}

//adds a value to the vector, difference is that it does merge every single time element is added
void ExtraPriorityQueue::enqueue(string value) {
	logLen++;
	nodeT* current = new nodeT;
	current->value = value;
	Vector<nodeT*> curVec;
	current->children = curVec;
	roots += current;
	merge();
}

//returns minimal element
string ExtraPriorityQueue::peek() {
	if(isEmpty()) error("Can not peek from an empty queue");

	string min = roots[0]->value;
	for (int i=1; i<roots.size(); i++) {
		if(roots[i]->value < min) min = roots[i]->value;
	}
	return min;
}

//promotes nodes, puts them in the main vector
void ExtraPriorityQueue::promoteNodes(int index) {
	nodeT* parent = roots.get(index);
	for (int i=0; i<parent->children.size(); i++) {
		roots += parent->children[i];
	}
}

//removes the minimal element
string ExtraPriorityQueue::dequeueMin() {
	if(isEmpty()) error("Can not dequeue from an empty queue");
	logLen--;
	string min = roots[0]->value;
	int index = 0;
	
	for (int i=1; i<roots.size(); i++) {
		if(roots[i]->value < min) {
			min = roots[i]->value;
			index = i;
		}
	}
	//promotes nodes before removing
	promoteNodes(index);
	roots.remove(index);
	merge();
	return min;
}