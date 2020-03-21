/*************************************************************
 * File: pqueue-vector.cpp
 *
 * Implementation file for the VectorPriorityQueue
 * class.
 */
 
#include "pqueue-vector.h"
#include "error.h"
#include <algorithm>

VectorPriorityQueue::VectorPriorityQueue() {
	
}

VectorPriorityQueue::~VectorPriorityQueue() {
	//we don't create any heap space, so no need for emptying
}

int VectorPriorityQueue::size() {	
	return vec.size();
}

bool VectorPriorityQueue::isEmpty() {
	return size()== 0;
}

void VectorPriorityQueue::enqueue(string value) {
	vec.add(value);
}

//iterates over vector and chooses minimal
string VectorPriorityQueue::peek() {
	if(isEmpty()) {
		error("Can not peek from an empty queue");
	}
	string minimal = vec[0];
	for (int i=0; i<vec.size(); i++) {
		if(minimal > vec[i]) {
			minimal = vec[i];
		}
	}
	return minimal;
}

//iterates over vector and removes minimal
string VectorPriorityQueue::dequeueMin() {
	if(isEmpty()) {
		error("Can not dequeue from an empty queue");
	}
	int index = 0; //saves the index
	string minimal = vec[0];
	for (int i=0; i<vec.size(); i++) {
		if(minimal > vec[i]) {
			minimal = vec[i];
			index = i;
		}
	}
	vec.remove(index);
	return minimal;
}