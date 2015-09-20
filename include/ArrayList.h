#ifndef _C_KWAZ_ARRAY_LIST_IMPL_
#define _C_KWAZ_ARRAY_LIST_IMPL_

#include <stdlib.h>
#include "Boolean.h"
#include "Iterator.h"

typedef struct ArrayList
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
} ArrayList;

/* Start extern for c++ */
#ifdef __cplusplus
extern "C" {
#endif

ArrayList* ArrayList_createDefault(const size_t pTypeSize);
ArrayList* ArrayList_create(
	const size_t pTypeSize,
	int (* const pCompare)(const void* first, const void* second, size_t size),
	void* (* const pCopy)(void * dest, const void * src, size_t size)
);
ArrayList* ArrayList_createFull(
	const size_t pTypeSize,
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor,
	int (* const pCompare)(const void* first, const void* second, size_t size),
	void* (* const pCopy)(void * dest, const void * src, size_t size)
);

/* ArrayList Methods */
void ArrayList_destroy(ArrayList* pList);
ArrayList* ArrayList_add(ArrayList* pList, const void* pValue);
ArrayList* ArrayList_addAll(ArrayList* pList, const ArrayList* pOtherList);
ArrayList* ArrayList_addIterator(ArrayList* pList, Iterator* pIter);
ArrayList* ArrayList_remove(ArrayList* pList, const size_t pIndex);
void* ArrayList_get(const ArrayList* pList, const size_t pIndex);
ArrayList* ArrayList_set(ArrayList* pList, const void* value, const size_t pIndex);
Boolean ArrayList_contains(const ArrayList* pList, const void* pValue);
ArrayList* ArrayList_mergeSort(ArrayList* pList);
Iterator* ArrayList_iterator(ArrayList* pList);


/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
