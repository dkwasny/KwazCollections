#ifndef _C_KWAZ_ARRAY_LIST_IMPL_
#define _C_KWAZ_ARRAY_LIST_IMPL_

#include <stdlib.h>
#include "Boolean.h"

typedef struct ArrayList
{
    int* values;
    size_t size;
    size_t capacity;

    size_t initialCapacity;
    unsigned int addReallocationMultiplier;
    unsigned int removeReallocationThreshold;
    unsigned int removeReallocationDivisor;
} ArrayList;

/* Start extern for c++ */
#ifdef __cplusplus
extern "C" {
#endif

ArrayList* ArrayList_create(void);
ArrayList* ArrayList_createFull(
    const size_t pCapacity,
    const unsigned int pAddReallocationMultiplier,
    const unsigned int pRemoveReallocationThreshold,
    const unsigned int pRemoveReallocationDivisor
);

/* ArrayList Methods */
void ArrayList_destroy(ArrayList* pList);
ArrayList* ArrayList_add(ArrayList* pList, const int pValue);
ArrayList* ArrayList_addAll(ArrayList* pList, const ArrayList* pOtherList);
ArrayList* ArrayList_remove(ArrayList* pList, const size_t pIndex);
int ArrayList_get(const ArrayList* pList, const size_t pIndex);
ArrayList* ArrayList_set(ArrayList* pList, const int pValue, const size_t pIndex);
Boolean ArrayList_contains(const ArrayList* pList, const int pValue);
ArrayList* ArrayList_mergeSort(ArrayList* pList);

/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
