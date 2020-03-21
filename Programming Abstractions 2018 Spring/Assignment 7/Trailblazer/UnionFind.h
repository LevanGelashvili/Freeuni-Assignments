#pragma once

/*EXTENSION
	I implemented myself a union find algorithm, which makes clusterization in O(1) time approximately.
	We have two arrays, of parents and their sizes. Firstly, parent array contains every single element, size is 
	set to 1 for all. Next, if we merge clusters, we assign parent of a second cluster as first, and increase its size
	by elements added. This way is way more efficient than the original one (with cluster sets), but i still wrote both.
*/

class UnionFind {
public:
	UnionFind(int n);
	~UnionFind();
	int find(int elem);
	void merge(int a, int b);
	bool areMerged(int a, int b);
private:
	//int array of element sizes
	int *sizes;
	//int array of elements themselves
	int *parents;
};
