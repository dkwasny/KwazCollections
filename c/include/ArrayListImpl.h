#ifndef _C_KWAZ_ARRAY_LIST_IMPL_
#define _C_KWAZ_ARRAY_LIST_IMPL_

#include <stdlib.h>
#include "Boolean.h"

typedef struct ArrayListImpl ArrayListImpl;
typedef struct ArrayListImplIterator ArrayListImplIterator;

struct ArrayListImpl
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
};

struct ArrayListImplIterator
{
	size_t nextIndex;
	ArrayListImpl* list;
};

/* Start extern for c++ */
#ifdef __cplusplus
extern "C" {
#endif

ArrayListImpl* ArrayListImpl_createDefault(const size_t pTypeSize);
ArrayListImpl* ArrayListImpl_create(
	const size_t pTypeSize,
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor
);
ArrayListImpl* ArrayListImpl_createCompare(
	const size_t pTypeSize,
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor,
	int (* pCompare)(const void* first, const void* second, size_t size)
);

/* ArrayList Methods */
void ArrayListImpl_destroy(ArrayListImpl* pList);
void ArrayListImpl_add(ArrayListImpl* pList, const void* pValue);
void ArrayListImpl_addAll(ArrayListImpl* pList, const ArrayListImpl* pOtherList);
void ArrayListImpl_remove(ArrayListImpl* pList, const size_t pIndex);
void* ArrayListImpl_get(const ArrayListImpl* pList, const size_t pIndex);
Boolean ArrayListImpl_contains(const ArrayListImpl* pList, const void* pValue);
ArrayListImplIterator* ArrayListImpl_iterator(ArrayListImpl* pList);

/* Iterator Methods */
Boolean ArrayListImplIterator_hasNext(ArrayListImplIterator* pIter);
void* ArrayListImplIterator_peekNext(ArrayListImplIterator* pIter);
void* ArrayListImplIterator_next(ArrayListImplIterator* pIter);
void ArrayListImplIterator_remove(ArrayListImplIterator* pIter);

void ArrayListImplIterator_destroy(ArrayListImplIterator* pIter);

/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
