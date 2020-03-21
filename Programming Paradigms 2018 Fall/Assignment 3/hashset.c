#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//Creates a new hashset, code in this c.file is short, because vector does most of the work all by itself
void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn){
	assert(elemSize > 0 && numBuckets > 0 && hashfn != NULL && comparefn != NULL);
	h->elemSize = elemSize;
	h->logLen = 0;
	h->numBuckets = numBuckets;
	h->hashFn = hashfn;
	h->hashFreeFn = freefn;
	h->hashCmpFn = comparefn;
	h->elems = malloc(numBuckets * sizeof(vector));
	assert(h->elems != NULL);
	//Every bucket is a vector, so we create array of vector to make a hashset
	for (int i=0; i < numBuckets; i++) {
		VectorNew((vector*)h->elems + i,h->elemSize,h->hashFreeFn,1);
	}
}

//Frees hashSet with given function
void HashSetDispose(hashset *h){
	for (int i=0; i<h->numBuckets; i++) {
		VectorDispose((vector*)h->elems + i);
	}
	free(h->elems);
}

//Returns size
int HashSetCount(const hashset *h){ 
	return h->logLen;
}

//Maps elements of hashset with a given function
void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData){
	assert(mapfn != NULL);
	for (int i=0; i<h->numBuckets; i++) {
		vector* vec = (vector*)h->elems + i;
		VectorMap(vec,mapfn,auxData);
	}
}

//Searches for an element and replaces it if present, otherwise appends it to a specific vector
//(determined by hashFunction)
void HashSetEnter(hashset *h, const void *elemAddr){
	assert(elemAddr != NULL);
	int bucket = h->hashFn(elemAddr,h->numBuckets);
	assert(bucket >= 0 && bucket < h->numBuckets);

	vector* vec = (vector*)h->elems + bucket;
	int index = VectorSearch(vec,elemAddr,h->hashCmpFn,0,false);
	if(index == -1) {
		VectorAppend(vec,elemAddr);
		h->logLen++;
	}
	else VectorReplace(vec,elemAddr,index);
}

//Same logic here, but returns pointer of an element
void *HashSetLookup(const hashset *h, const void *elemAddr){ 
	assert(elemAddr != NULL);
	int bucket = h->hashFn(elemAddr,h->numBuckets);
	assert(bucket >= 0 && bucket < h->numBuckets);

	vector* vec = (vector*)h->elems + bucket;
	int index = VectorSearch(vec,elemAddr,h->hashCmpFn,0,false);
	if(index == -1) return NULL;
	else return VectorNth(vec,index);
}