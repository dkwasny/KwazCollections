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
} ArrayList;

/* Start extern for c++ */
#ifdef __cplusplus
extern "C" {
#endif

ArrayList* ArrayList_createDefault(const size_t pTypeSize);
ArrayList* ArrayList_create(
	const size_t pTypeSize,
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor
);
ArrayList* ArrayList_createCompare(
	const size_t pTypeSize,
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor,
	int (* pCompare)(const void* first, const void* second, size_t size)
);

/* ArrayList Methods */
void ArrayList_destroy(ArrayList* pList);
void ArrayList_add(ArrayList* pList, const void* pValue);
void ArrayList_addAll(ArrayList* pList, const ArrayList* pOtherList);
void ArrayList_remove(ArrayList* pList, const size_t pIndex);
void* ArrayList_get(const ArrayList* pList, const size_t pIndex);
Boolean ArrayList_contains(const ArrayList* pList, const void* pValue);
Iterator* ArrayList_iterator(ArrayList* pList);

/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
