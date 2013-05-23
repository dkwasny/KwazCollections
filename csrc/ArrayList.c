#include "ArrayList.h"

ArrayList* ArrayList_createDefault()
{
	ArrayList* retVal;
	int* values = (int*)malloc(sizeof(int) * ARRAY_LIST_INITIAL_CAPACITY);
	ArrayList tmpVal = {
		NULL,
		0,
		ARRAY_LIST_INITIAL_CAPACITY,
		ARRAY_LIST_INITIAL_CAPACITY,
		ARRAY_LIST_ADD_REALLOCATION_THRESHOLD,
		ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER,
		ARRAY_LIST_REMOVE_REALLOCATION_THRESHOLD,
		ARRAY_LIST_REMOVE_REALLOCATION_MULTIPLIER
	};
	tmpVal.values = values;

	retVal = (ArrayList*) malloc(sizeof(ArrayList));
	memcpy(retVal, &tmpVal, sizeof(ArrayList));
	return retVal;
}

/* See header file for more info
 *
ArrayList* ArrayList_createCustom(
	const size_t pInitialCapacity,
	const float pAddReallocationThreshold,
	const unsigned short pAddReallocationMultiplier,
	const float pRemoveReallocationThreshold,
	const float pRemoveReallocationMultiplier)
{
	ArrayList* retVal;
	int* values = (int*)malloc(sizeof(int) * pInitialCapacity);
	ArrayList tmpVal = {
		NULL,
		0,
		pInitialCapacity,
		pInitialCapacity,
		pAddReallocationThreshold,
		pAddReallocationMultiplier,
		pRemoveReallocationThreshold,
		pRemoveReallocationMultiplier
	};
	tmpVal.values = values;

	retVal = (ArrayList*) malloc(sizeof(ArrayList));
	memcpy(retVal, &tmpVal, sizeof(ArrayList));
	return retVal;
}*/

ArrayList* ArrayList_createCopy(const ArrayList* pOther)
{
	ArrayList* retVal = (ArrayList*) malloc(sizeof(ArrayList));
	int* values;
	ArrayList tmpVal = {
		NULL,
		0,
		0,
		ARRAY_LIST_INITIAL_CAPACITY,
		ARRAY_LIST_ADD_REALLOCATION_THRESHOLD,
		ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER,
		ARRAY_LIST_REMOVE_REALLOCATION_THRESHOLD,
		ARRAY_LIST_REMOVE_REALLOCATION_MULTIPLIER
	};
	values = ArrayList_allocateArray(pOther->values, pOther->size, pOther->capacity);
	tmpVal.values = values;
	tmpVal.capacity = pOther->capacity;
	tmpVal.size = pOther->size;
	memcpy(retVal, &tmpVal, sizeof(ArrayList));

	return retVal;
}

void ArrayList_destroy(ArrayList* pList)
{
	free(pList->values);
}

void ArrayList_add(ArrayList* pList, const int pValue) {
	/* Expand the array if we pass a specified threshold */
	if ((pList->capacity * pList->addReallocationThreshold) <= pList->size) {
		size_t newCapacity = pList->capacity * pList->addReallocationMultiplier;
		int* newValues;
		if (newCapacity == 0) { newCapacity++; }  /* Gotta handle 0... */
		newValues = ArrayList_allocateArray(pList->values, pList->size, newCapacity);
		free(pList->values);
		pList->values = newValues;
		pList->capacity = newCapacity;
	}

	pList->values[pList->size++] = pValue;
}

int ArrayList_remove(ArrayList* pList, const size_t pIndex) {	
	int removedVal = 0;
	size_t i = 0;
	size_t newCapacity = 0;
	
	if (pIndex >= pList->size) {
		/*TODO: Need some sort of return code to prevent this */
	}

	removedVal = pList->values[pIndex];
	
	--pList->size;
	for (i = pIndex; i < pList->size; ++i) {
		pList->values[i] = pList->values[i + 1];
	}
	
	/* Shrink the array if we are only using a small portion of it.
	 * To prevent non-stop resizing, the array will not be shrunk
	 * past its initial capacity.
	 * I understand that most lists do not do this, but I
	 * wanted to do this anyways.
	 */
	newCapacity = pList->capacity * pList->removeReallocationMultiplier;
	if (newCapacity >= pList->initialCapacity 
		&& (pList->capacity * pList->removeReallocationThreshold) >= pList->size)
	{
		int* newValues = ArrayList_allocateArray(pList->values, pList->size, newCapacity);
		free(pList->values);
		pList->values = newValues;
		pList->capacity = newCapacity;
	}

	return removedVal;	
}

int* ArrayList_allocateArray(
	const int* pOrigValues,
	const size_t pOrigValuesSize,
	const size_t pNewCapacity)
{
	size_t valuesSize;
	int* retVal;

	if (pOrigValuesSize > pNewCapacity) {
		/* TODO: Return error code or something*/
	}

	valuesSize = sizeof(int) * pNewCapacity;
	retVal = (int*)malloc(valuesSize);
	memcpy(retVal, pOrigValues, valuesSize);

	return retVal;
}
