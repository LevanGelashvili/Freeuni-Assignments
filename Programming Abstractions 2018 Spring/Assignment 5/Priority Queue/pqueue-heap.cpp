/*************************************************************
 * File: pqueue-heap.cpp
 *
 * Implementation file for the HeapPriorityQueue
 * class.
 */
 
#include "pqueue-heap.h"
#include "error.h"

//iniatilizes variables
HeapPriorityQueue::HeapPriorityQueue() {
	logLen = 0;
	allocLen = 8;
	data = new string[allocLen];
}

//frees allocated space
HeapPriorityQueue::~HeapPriorityQueue() {
	delete[] data;
}

int HeapPriorityQueue::size() {
	return logLen;	
}

bool HeapPriorityQueue::isEmpty() {
	return size() == 0;
}

void HeapPriorityQueue::enqueue(string value) {
	if(isEmpty()) {
		//inserts element at 0, if queue is empty
		data[0] = value;
		logLen++;
		return;
	}
	if(logLen >= allocLen) {
		//we allocate memory, if our length exceeds the allocated array
		allocateMemory();
	}
	int index = logLen;
	logLen++;
	data[index] = value;
	bubbleUp(index);
}

//doubles allocated memory, copies data in a new array and deletes the last one
void HeapPriorityQueue::allocateMemory() {
	allocLen = allocLen * 2;
	string* temp = new string[allocLen];
	for (int i=0; i<logLen; i++) {
		temp[i] = data[i];
	}
	delete[] data;
	data = temp;
}

//this method checks if child value is bigger than parent's, if so, swaps nodes
void HeapPriorityQueue::bubbleUp(int index) {
	if(index <= 0) return;

	int parent;
	//index/2 and index/2-1 repserent parent indexes
	if(index%2 == 0) parent = index/2 - 1;
	else parent = index/2;
	if (index <=2) parent = 0;

	if(data[parent] > data[index]) {
		swapNodes(index,parent);
	}
	bubbleUp(parent);
}

//swaps values of two nodes
void HeapPriorityQueue::swapNodes(int index, int parent) {
	string temp = data[parent];
	data[parent] = data[index];
	data[index] = temp;
}

//returns min element
string HeapPriorityQueue::peek() {
	if(isEmpty()) error("Can not peek from empty queue");
	return data[0];
}

//removes min element, at 0, and bubblesDown if needed so
string HeapPriorityQueue::dequeueMin() {
	if(isEmpty()) error("Can not dequeue from empty queue");
	string result = data[0];
	swapNodes(0,logLen-1);
	logLen--;
	bubbleDown(0);
	return result;
}

//exact logic as bubble up, swaps root value down
void HeapPriorityQueue::bubbleDown(int index) {
	if(index*2+1 >= logLen) return;

	int minChild;
	if(index*2+2 >= logLen) minChild = index*2+1;
	else {
		if(data[index*2+1] < data[index*2+2]) minChild = index*2+1;
		else minChild = index*2+2;
	}
	if(data[minChild] < data[index]) {
		swapNodes(index,minChild);
	}
	bubbleDown(minChild);
}