#include "monika.h"
#include "threads/malloc.h"
#include <string.h>
void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
    v->_elemSize = elemSize;
    v->_allocLen = initialAllocation;
    if (initialAllocation == 0)
        v->_allocLen = 5;
    v->_logLen = 0;
    v->_freeFunc = freeFn;
    v->_arr = malloc(v->_allocLen * v->_elemSize);
}

void VectorDispose(vector *v)
{
    if (v->_freeFunc != NULL)
    {
        int i=0;
        for (i = 0; i < v->_logLen; i++)
        {
            v->_freeFunc((char *)v->_arr + i * v->_elemSize);
        }
    }
    free(v->_arr);
}
void VectorIncreaseSize(vector *v)
{
    v->_allocLen += 20;
    v->_arr = realloc(v->_arr, v->_allocLen * v->_elemSize);
}

int VectorLength(const vector *v)
{
    return v->_logLen;
}

void *VectorNth(const vector *v, int position)
{
    return (char *)v->_arr + position * v->_elemSize;
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
    if (v->_freeFunc != NULL)
        v->_freeFunc((char *)v->_arr + position * v->_elemSize);
    memcpy((char *)v->_arr + position * v->_elemSize, elemAddr, v->_elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
    if (v->_logLen == v->_allocLen)
        VectorIncreaseSize(v);
    memmove((char *)v->_arr + (position + 1) * v->_elemSize, (char *)v->_arr + position * v->_elemSize, (v->_logLen - position) * v->_elemSize);
    memcpy((char *)v->_arr + position * v->_elemSize, elemAddr, v->_elemSize);
    v->_logLen++;
}

void VectorAppend(vector *v, const void *elemAddr)
{
    if (v->_logLen == v->_allocLen)
        VectorIncreaseSize(v);
    memcpy((char *)v->_arr + v->_logLen * v->_elemSize, elemAddr, v->_elemSize);
    v->_logLen++;
}

void VectorDelete(vector *v, int position)
{
    if (v->_freeFunc != NULL)
        v->_freeFunc((char *)v->_arr + position * v->_elemSize);
    memmove((char *)v->_arr + position * v->_elemSize, (char *)v->_arr + (position + 1) * v->_elemSize, (v->_logLen - position - 1) * v->_elemSize);
    v->_logLen--;
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
    int i=0;
    for (i = 0; i < v->_logLen; i++)
        mapFn((char *)v->_arr + i * v->_elemSize, auxData);
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex)
{
    int i=0;
    for (i = startIndex; i < v->_logLen; i++)
        if (searchFn((char *)v->_arr + i * v->_elemSize, key) == 0)
            return i;
    return kNotFound;
}
