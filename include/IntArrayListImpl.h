#ifndef _C_KWAZ_INT_ARRAY_LIST_IMPL_
#define _C_KWAZ_INT_ARRAY_LIST_IMPL_

#include "ArrayListImpl.h"
#include <stdlib.h>

typedef struct IntArrayListImpl
{
	ArrayListImpl* list;
} IntArrayListImpl;

/* Start extern for c++ */
#ifdef __cplusplus
extern "C" {
#endif

IntArrayListImpl* IntArrayListImpl_create(
	ArrayListImpl* pList
);

void IntArrayListImpl_destroy(IntArrayListImpl* pList);
void IntArrayListImpl_add(IntArrayListImpl* pList, const int pValue);
void IntArrayListImpl_addAll(IntArrayListImpl* pList, const IntArrayListImpl* pOtherList);
void IntArrayListImpl_remove(IntArrayListImpl* pList, const size_t pIndex);
int IntArrayListImpl_get(const IntArrayListImpl* pList, const size_t pIndex);
Boolean IntArrayListImpl_contains(const IntArrayListImpl* pList, const int pValue);

/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
