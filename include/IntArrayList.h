#ifndef _C_KWAZ_INT_ARRAY_LIST_IMPL_
#define _C_KWAZ_INT_ARRAY_LIST_IMPL_

#include "ArrayList.h"
#include "Boolean.h"
#include <stdlib.h>

typedef struct IntArrayList
{
	ArrayList* list;
} IntArrayList;

/* Start extern for c++ */
#ifdef __cplusplus
extern "C" {
#endif

IntArrayList* IntArrayList_create(
	ArrayList* pList
);

void IntArrayList_destroy(IntArrayList* pList);
void IntArrayList_add(IntArrayList* pList, const int pValue);
void IntArrayList_addAll(IntArrayList* pList, const IntArrayList* pOtherList);
void IntArrayList_remove(IntArrayList* pList, const size_t pIndex);
int IntArrayList_get(const IntArrayList* pList, const size_t pIndex);
Boolean IntArrayList_contains(const IntArrayList* pList, const int pValue);

/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
