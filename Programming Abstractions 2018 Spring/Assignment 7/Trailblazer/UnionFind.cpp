#include "UnionFind.h"
#include "error.h"

//Initializes arrays of n elements
UnionFind::UnionFind(int n) {
	sizes = new int[n];
	parents = new int[n];
	for (int i=0; i<n; i++) {
		sizes[i] = 1;
		parents[i] = i;
	}
}

//Frees space
UnionFind::~UnionFind() {
	delete[]sizes;
	delete[]parents;
}

//Checks if two ints are in a same component
bool UnionFind::areMerged(int a, int b) {
	return find(a) == find(b);
}

//Searches for an element in an array
int UnionFind::find(int elem) {
	int copy = elem;
	//Iterates in a array, until reaches original parent of an int
	while(copy != parents[copy]) copy = parents[copy];

	//Restores the paths, we destroyed while searching for a parent
	while(elem != copy) {
		int n = parents[elem];
		n = copy;
		elem = copy;
	}
	return copy;
}

//Merges two clusters
void UnionFind::merge(int a, int b) {
	//No need to do it if they are already merged
	if(areMerged(a,b)) return;

	//Gets parents of ints
	int i = find(a);
	int j = find(b);

	//Checks which is smaller, to add it to a larger one, whole process being assigning larger one as its parent
	if(sizes[i] < sizes[j]) {
		parents[i] = j;
		sizes[j] += sizes[i];
	} else {
		parents[j] = i;
		sizes[i] += sizes[j];
	}
}
