#include "ArrayList.h"

ArrayList* ArrayList_createDefault()
{
	ArrayList* retVal;
	int* values = (int*)malloc(sizeof(int) * ARRAY_LIST_INITIAL_CAPACITY);
	ArrayList tmpVal = {
		NULL,
		0,
		ARRAY_LIST_INITIAL_CAPACITY
	};
	tmpVal.values = values;

	retVal = (ArrayList*) malloc(sizeof(ArrayList));
	memcpy(retVal, &tmpVal, sizeof(ArrayList));
	return retVal;
}

ArrayList* ArrayList_createCopy(const ArrayList* pOther)
{
	ArrayList* retVal = (ArrayList*) malloc(sizeof(ArrayList));
	int* values;
	ArrayList tmpVal = {
		NULL,
		0,
		0
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
	if (pList->capacity == pList->size) {
		size_t newCapacity = pList->capacity * ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER;
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
	newCapacity = pList->capacity / ARRAY_LIST_REMOVE_REALLOCATION_DIVISOR;
	if (newCapacity >= ARRAY_LIST_INITIAL_CAPACITY
		&& (pList->size * ARRAY_LIST_REMOVE_REALLOCATION_THRESHOLD) < pList->capacity)
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
	size_t origValuesBytes;
	size_t retValBytes;
	int* retVal;

	if (pOrigValuesSize > pNewCapacity) {
		/* TODO: Return error code or something*/
	}

	origValuesBytes = sizeof(int) * pOrigValuesSize;
	retValBytes = sizeof(int) * pNewCapacity;
	retVal = (int*)malloc(retValBytes);
	memcpy(retVal, pOrigValues, origValuesBytes);

	return retVal;
}
