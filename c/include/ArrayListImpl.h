#ifndef _C_KWAZ_ARRAY_LIST_IMPL_
#define _C_KWAZ_ARRAY_LIST_IMPL_

#include <stdlib.h>
#include <string.h>

typedef struct ArrayListImpl
{
	int* values;
	size_t size;
	size_t capacity;

	size_t initialCapacity;
	unsigned int addReallocationMultiplier;
	unsigned int removeReallocationThreshold;
	unsigned int removeReallocationDivisor;
} ArrayListImpl;

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

void ArrayListImpl_delete(ArrayListImpl* pList);
void ArrayListImpl_add(ArrayListImpl* pList, const int pValue);
int ArrayListImpl_remove(ArrayListImpl* pList, const size_t pIndex);
int ArrayListImpl_get(const ArrayListImpl* pList, const size_t pIndex);

/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
