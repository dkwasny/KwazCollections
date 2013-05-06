#include "ArrayList.h"

ArrayList* ArrayList_createDefault()
{
	int* values = (int*)malloc(sizeof(int) * ARRAY_LIST_INITIAL_CAPACITY);
	ArrayList tmpVal = {
		values,
		0,
		ARRAY_LIST_INITIAL_CAPACITY,
		ARRAY_LIST_INITIAL_CAPACITY,
		ARRAY_LIST_ADD_REALLOCATION_THRESHOLD,
		ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER,
		ARRAY_LIST_REMOVE_REALLOCATION_THRESHOLD,
		ARRAY_LIST_REMOVE_REALLOCATION_MULTIPLIER
	};

	ArrayList* retVal = (ArrayList*) malloc(sizeof(ArrayList));
	memcpy(retVal, &tmpVal, sizeof(ArrayList));
	return retVal;
}

ArrayList* ArrayList_createCustom(
	const unsigned int pInitialCapacity,
	const float pAddReallocationThreshold,
	const unsigned short pAddReallocationMultiplier,
	const float pRemoveReallocationThreshold,
	const float pRemoveReallocationMultiplier)
{
	int* values = (int*)malloc(sizeof(int) * pInitialCapacity);
	ArrayList tmpVal = {
		values,
		0,
		pInitialCapacity,
		pInitialCapacity,
		pAddReallocationThreshold,
		pAddReallocationMultiplier,
		pRemoveReallocationThreshold,
		pRemoveReallocationMultiplier
	};

	ArrayList* retVal = (ArrayList*) malloc(sizeof(ArrayList));
	memcpy(retVal, &tmpVal, sizeof(ArrayList));
	return retVal;
}

ArrayList* ArrayList_createCopy(const ArrayList* pOther)
{
	int* values = ArrayList_allocateArray(pOther->values, pOther->size, pOther->capacity);
	ArrayList tmpVal = {
		values,
		pOther->size,
		pOther->capacity,
		pOther->initialCapacity,
		pOther->addReallocationThreshold,
		pOther->addReallocationMultiplier,
		pOther->removeReallocationThreshold,
		pOther->removeReallocationMultiplier
	};

	ArrayList* retVal = (ArrayList*) malloc(sizeof(ArrayList));
	memcpy(retVal, &tmpVal, sizeof(ArrayList));
	return retVal;
}

void ArrayList_destroy(ArrayList* pList)
{
	free(pList->values);
}

void ArrayList_add(ArrayList* pList, const int pValue) {
	// Expand the array if we pass a specified threshold
	if ((pList->capacity * pList->addReallocationThreshold) <= pList->size) {
		int newCapacity = pList->capacity * pList->addReallocationMultiplier;
		if (newCapacity == 0) { newCapacity++; }  // Gotta handle 0...
		int* newValues = ArrayList_allocateArray(pList->values, pList->size, newCapacity);
		free(pList->values);
		pList->values = newValues;
		pList->capacity = newCapacity;
	}

	pList->values[pList->size++] = pValue;
}

int ArrayList_remove(ArrayList* pList, const unsigned int pIndex) {	
	if (pIndex >= pList->size) {
		//TODO: Need some sort of return code to prevent this
	}

	int removedVal = pList->values[pIndex];
	
	--pList->size;
	int i;
	for (i = pIndex; i < pList->size; ++i) {
		pList->values[i] = pList->values[i + 1];
	}
	
	// Shrink the array if we are only using a small portion of it.
	// To prevent non-stop resizing, the array will not be shrunk
	// past its initial capacity.
	// I understand that most lists do not do this, but I
	// wanted to do this anyways.
	int newCapacity = pList->capacity * pList->removeReallocationMultiplier;
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
	const unsigned int pOrigValuesSize,
	const unsigned int pNewCapacity)
{
	if (pOrigValuesSize > pNewCapacity) {
		// TODO: Return error code or something
	}

	size_t valuesSize = sizeof(int) * pNewCapacity;
	int* retVal = (int*)malloc(valuesSize);
	memcpy(retVal, pOrigValues, valuesSize);

	return retVal;
}
