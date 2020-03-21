#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>
#include <assert.h>

//Creates new vector
void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation){
    assert(elemSize > 0 && initialAllocation >= 0);
    v->logLen = 0;
    if(initialAllocation == 0) initialAllocation = 5;
    v->allocLen = initialAllocation;
    v->elemSize = elemSize;
    v->freeFn = freeFn;
    v->elems = malloc(v->allocLen * elemSize);
    assert(v->elems != NULL);
}

//Frees vector with provided freeFn
void VectorDispose(vector *v){
    if(v->freeFn != NULL) {
        for (int i = 0; i < v->logLen; i++) {
            v->freeFn((char*)v->elems + i * v->elemSize);
        }
    }
    free(v->elems);
}

//Size of vector
int VectorLength(const vector *v){ 
    return v->logLen;
}

//Returns pointer to vector's nth element
void *VectorNth(const vector *v, int position){ 
    assert(position >= 0 && position < v->logLen);
    return (void*)((char*)v->elems + position * v->elemSize);
}

//Replaces vector's position-th element with given element
void VectorReplace(vector *v, const void *elemAddr, int position){
    assert(position >= 0 && position < v->logLen);
    if(v->freeFn != NULL) v->freeFn((char*)v->elems + position * v->elemSize);
    memcpy((char*)v->elems + position * v->elemSize,elemAddr,v->elemSize);
}

//Grows allocated space. Only if logical lenth is equal to is allocated length
void grow(vector *v) {
    v->allocLen *= 2;
    v->elems = realloc(v->elems, v->allocLen * v->elemSize);
    assert(v->elems != NULL);
}

//Inserts element into a vector
void VectorInsert(vector *v, const void *elemAddr, int position){
    assert(position >= 0 && position <= v->logLen);
    if(v->logLen >= v->allocLen) grow(v);
    void* base = (char*)v->elems + position * v->elemSize;
    //We use memmove in case memory spaces overlap within each other
    memmove((char*)base + v->elemSize,base,v->elemSize * (v->logLen++ - position));
    memcpy(base,elemAddr,v->elemSize);
}

//Adds element at the end of vector
void VectorAppend(vector *v, const void *elemAddr){
    if(v->logLen >= v->allocLen) grow(v);
    memcpy((char*)v->elems + v->logLen++ * v->elemSize,elemAddr,v->elemSize);
}

//Removes element from vector, memmove needs to be used here too
void VectorDelete(vector *v, int position){
    assert(position >= 0 && position < v->logLen);
    void* base = (char*)v->elems + position * v->elemSize;
    if(v->freeFn != NULL) v->freeFn(base);
    memmove(base,(char*)base + v->elemSize,v->elemSize * (v->logLen-- - position - 1));
}

//Sorts vector
void VectorSort(vector *v, VectorCompareFunction compare){
    assert(compare != NULL);
    qsort(v->elems,v->logLen,v->elemSize,compare);
}

//Maps vector with provided mapFn
void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData){
    assert(mapFn != NULL);
    for (int i=0; i< VectorLength(v); i++) {
        void* cur = (char*)v->elems + i * v->elemSize;
        mapFn(cur,auxData);
    }
}

static const int kNotFound = -1;

//Searches for an element, returning its index. Uses lseach is not sorted, otherwise, bsearch
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, 
                int startIndex, bool isSorted){
    assert(startIndex >= 0 && startIndex <= v->logLen);
    assert(key != NULL && searchFn != NULL);
    void * search;
    void * base = (char*)v->elems + startIndex * v->elemSize;
    size_t size = v->logLen - startIndex;

    if(isSorted) search = bsearch(key,base,size,v->elemSize,searchFn);
    //lfind is a safer choice than lsearch
    else search = lfind(key,base,&size,v->elemSize,searchFn);
    if(search == NULL) return kNotFound;
    return ((char*)search - (char*)v->elems) / v->elemSize;
} 