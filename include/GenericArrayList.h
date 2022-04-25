#ifndef _C_KWAZ_GENERIC_ARRAY_LIST_IMPL_
#define _C_KWAZ_GENERIC_ARRAY_LIST_IMPL_

#include <stdlib.h>
#include "Boolean.h"
#include "GenericIterator.h"

/*
    This is from back when I was trying to do "true" generic collections in C.

    While messing with function pointers is fun, the goal of this project is
    to explore algorithms and data structures.

    I'm keeping this around as a reminder of what I was able to do, but will
    not be updating it any further.
    All further collection work will just assume primitive `int` values.
*/

typedef struct GenericArrayList
{
    void** values;
    size_t size;
    size_t capacity;

    size_t typeSize;
    size_t initialCapacity;
    unsigned int addReallocationMultiplier;
    unsigned int removeReallocationThreshold;
    unsigned int removeReallocationDivisor;

    int (* compare)(const void* first, const void* second, size_t size);
    void* (* copy)(void * dest, const void * src, size_t size);
} GenericArrayList;

/* Start extern for c++ */
#ifdef __cplusplus
extern "C" {
#endif

GenericArrayList* GenericArrayList_createDefault(const size_t pTypeSize);
GenericArrayList* GenericArrayList_create(
    const size_t pTypeSize,
    int (* const pCompare)(const void* first, const void* second, size_t size),
    void* (* const pCopy)(void * dest, const void * src, size_t size)
);
GenericArrayList* GenericArrayList_createFull(
    const size_t pTypeSize,
    const size_t pCapacity,
    const unsigned int pAddReallocationMultiplier,
    const unsigned int pRemoveReallocationThreshold,
    const unsigned int pRemoveReallocationDivisor,
    int (* const pCompare)(const void* first, const void* second, size_t size),
    void* (* const pCopy)(void * dest, const void * src, size_t size)
);

/* GenericArrayList Methods */
void GenericArrayList_destroy(GenericArrayList* pList);
GenericArrayList* GenericArrayList_add(GenericArrayList* pList, const void* pValue);
GenericArrayList* GenericArrayList_addAll(GenericArrayList* pList, const GenericArrayList* pOtherList);
GenericArrayList* GenericArrayList_addIterator(GenericArrayList* pList, GenericIterator* pIter);
GenericArrayList* GenericArrayList_remove(GenericArrayList* pList, const size_t pIndex);
void* GenericArrayList_get(const GenericArrayList* pList, const size_t pIndex);
GenericArrayList* GenericArrayList_set(GenericArrayList* pList, const void* value, const size_t pIndex);
Boolean GenericArrayList_contains(const GenericArrayList* pList, const void* pValue);
GenericArrayList* GenericArrayList_mergeSort(GenericArrayList* pList);
GenericIterator* GenericArrayList_iterator(GenericArrayList* pList);

/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
