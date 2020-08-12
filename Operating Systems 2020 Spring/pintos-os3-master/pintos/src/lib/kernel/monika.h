#ifndef _vector_
#define _vector_
typedef int (*VectorCompareFunction)(const void *elemAddr1, const void *elemAddr2);
typedef void (*VectorMapFunction)(void *elemAddr, void *auxData);
typedef void (*VectorFreeFunction)(void *elemAddr);
typedef struct
{
  void *_arr;
  int _allocLen;
  int _logLen;
  int _elemSize;
  VectorFreeFunction _freeFunc;
} vector;
void VectorIncreaseSize(vector *v);
void VectorNew(vector *v, int elemSize, VectorFreeFunction freefn, int initialAllocation);
void VectorDispose(vector *v);
int VectorLength(const vector *v);
void *VectorNth(const vector *v, int position);
void VectorInsert(vector *v, const void *elemAddr, int position);
void VectorAppend(vector *v, const void *elemAddr);
void VectorReplace(vector *v, const void *elemAddr, int position);
void VectorDelete(vector *v, int position);
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchfn, int startIndex);
void VectorMap(vector *v, VectorMapFunction mapfn, void *auxData);
#endif
