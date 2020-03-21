/**********************************************
 * File: pqueue-extra.h
 *
 * Your own custom priority queue class, if you
 * want to build one for extra credit.
 *
 * Please replace these file comments with a
 * description of how your priority queue works.
 */
#ifndef PQueue_Extra_Included
#define PQueue_Extra_Included

#include <string>
#include "vector.h"
using namespace std;

/*
 * This class acts like fibonacci heap but is a little bit slower, due to merge operation, which we do after element is added or removed (contrary to only removing element in fib).
We dont have other pointers too, to keep track of minimal element and speed to process up.
 */
class ExtraPriorityQueue {
public:
	/* Constructs a new, empty priority queue. */
	ExtraPriorityQueue();
	
	/* Cleans up all memory allocated by this priority queue. */
	~ExtraPriorityQueue();
	
	/* Returns the number of elements in the priority queue. */
	int size();
	
	/* Returns whether or not the priority queue is empty. */
	bool isEmpty();
	
	/* Enqueues a new string into the priority queue. */
	void enqueue(string value);
	
	/* Returns, but does not remove, the lexicographically first string in the
	 * priority queue.
	 */
	string peek();
	
	/* Returns and removes the lexicographically first string in the
	 * priority queue.
	 */
	string dequeueMin();

private:
	// TODO: Fill this in with the implementation of your custom
	// priority queue.  You can add any fields, types, or methods that you
	// wish.
	struct nodeT{
		string value;
		Vector<nodeT*> children;
	};
	int logLen; //Logical length
	Vector<nodeT*> roots; // main vector of nodes

	void cleanUp(nodeT* node);
	void mergeNodes(nodeT* first, nodeT* second, int firstIndex);
	void promoteNodes(int index);
	void merge();
};

#endif
