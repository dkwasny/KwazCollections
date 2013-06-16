#ifndef _C_KWAZ_ARRAY_LIST_IMPL_
#define _C_KWAZ_ARRAY_LIST_IMPL_

#include <stdlib.h>
#include <string.h>
#include "Boolean.h"

typedef struct ArrayListImpl ArrayListImpl;
typedef struct ArrayListImplIterator ArrayListImplIterator;

struct ArrayListImpl
{
	int* values;
	size_t size;
	size_t capacity;

	size_t initialCapacity;
	unsigned int addReallocationMultiplier;
	unsigned int removeReallocationThreshold;
	unsigned int removeReallocationDivisor;
};

struct ArrayListImplIterator
{
	size_t currIndex;
	Boolean firstOperation;
	ArrayListImpl* list;
};

/* Start extern for c++ */
#ifdef __cplusplus
extern "C" {
#endif

ArrayListImpl* ArrayListImpl_createDefault();
ArrayListImpl* ArrayListImpl_create(
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor
);

/* ArrayList Methods */
void ArrayListImpl_delete(ArrayListImpl* pList);
void ArrayListImpl_add(ArrayListImpl* pList, const int pValue);
int ArrayListImpl_remove(ArrayListImpl* pList, const size_t pIndex);
int ArrayListImpl_get(const ArrayListImpl* pList, const size_t pIndex);
ArrayListImplIterator* ArrayListImpl_iterator(ArrayListImpl* pList);

/* Iterator Methods */
Boolean ArrayListImplIterator_hasNext(ArrayListImplIterator* pIter);
int ArrayListImplIterator_peekNext(ArrayListImplIterator* pIter);
int ArrayListImplIterator_next(ArrayListImplIterator* pIter);

int ArrayListImplIterator_current(ArrayListImplIterator* pIter);

Boolean ArrayListImplIterator_hasPrevious(ArrayListImplIterator* pIter);
int ArrayListImplIterator_peekPrevious(ArrayListImplIterator* pIter);
int ArrayListImplIterator_previous(ArrayListImplIterator* pIter);

void ArrayListImplIterator_delete(ArrayListImplIterator* pIter);

/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
