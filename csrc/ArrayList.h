#ifndef _KWAZ_ARRAY_LIST_
#define _KWAZ_ARRAY_LIST_

// Default configuration values
#define ARRAY_LIST_INITIAL_CAPACITY 10
#define ARRAY_LIST_ADD_REALLOCATION_THRESHOLD 0.75f
#define ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER 2
#define ARRAY_LIST_REMOVE_REALLOCATION_THRESHOLD 0.25f
#define ARRAY_LIST_REMOVE_REALLOCATION_MULTIPLIER 0.5f

#include <stdlib.h>
#include <string.h>

typedef struct {
	int* values;
	unsigned int size;
	unsigned int capacity;

	// Configuration constants
	const unsigned int initialCapacity;
	const float addReallocationThreshold;
	const unsigned short addReallocationMultiplier;
	const float removeReallocationThreshold;
	const float removeReallocationMultiplier; 
} ArrayList;

ArrayList* ArrayList_createDefault();
ArrayList* ArrayList_createCustom(
	const unsigned int pInitialCapacity,
	const float pAddReallocationThreshold,
	const unsigned short pAddReallocationMultiplier,
	const float pRemoveReallocationThreshold,
	const float pRemoveReallocationMultiplier);
ArrayList* ArrayList_createCopy(const ArrayList* pOther);

void ArrayList_destroy(ArrayList* pList);
// TODO: Maybe a deep destroy?  Frees all entries wuthin the list too?
	
void ArrayList_add(ArrayList* pList, const int pValue);
int ArrayList_remove(ArrayList* pList, const unsigned int pIndex);

// Internal methods
int* ArrayList_allocateArray(
	const int* pOrigValues,
	const unsigned int pOrigValuesSize,
	const unsigned int pNewCapacity
);

#endif
