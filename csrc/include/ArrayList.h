#ifndef _KWAZ_ARRAY_LIST_
#define _KWAZ_ARRAY_LIST_

/* Default configuration values */
#define ARRAY_LIST_INITIAL_CAPACITY 10
#define ARRAY_LIST_ADD_REALLOCATION_THRESHOLD 0.75f
#define ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER 2
#define ARRAY_LIST_REMOVE_REALLOCATION_THRESHOLD 0.25f
#define ARRAY_LIST_REMOVE_REALLOCATION_MULTIPLIER 0.5f

#include <stdlib.h>
#include <string.h>

typedef struct {
	int* values;
	size_t size;
	size_t capacity;

	/* Configuration constants */
	const size_t initialCapacity;
	const float addReallocationThreshold;
	const unsigned short addReallocationMultiplier;
	const float removeReallocationThreshold;
	const float removeReallocationMultiplier; 
} ArrayList;

/* Start extern for c++ */
#ifdef __cplusplus
extern "C" {
#endif

ArrayList* ArrayList_createDefault();

/* I am currently punting on this "constructor".
 * I cannot get it to satisfy pedantic AND maintain my const members.
 * I will revisit when I have time and/or need for it.
 *
 * This is due to: "initializer element is not computable at load time"
 *
ArrayList* ArrayList_createCustom(
	const size_t pInitialCapacity,
	const float pAddReallocationThreshold,
	const unsigned short pAddReallocationMultiplier,
	const float pRemoveReallocationThreshold,
	const float pRemoveReallocationMultiplier);*/

ArrayList* ArrayList_createCopy(const ArrayList* pOther);

void ArrayList_destroy(ArrayList* pList);
/* TODO: Maybe a deep destroy?  Frees all entries wuthin the list too? */
	
void ArrayList_add(ArrayList* pList, const int pValue);
int ArrayList_remove(ArrayList* pList, const size_t pIndex);

/* Internal methods */
int* ArrayList_allocateArray(
	const int* pOrigValues,
	const size_t pOrigValuesSize,
	const size_t pNewCapacity
);

/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
