#include "ArrayList.h"

ICollection* ArrayList_ICollection_create()
{
	ICollection tmpVal = {
		ArrayList_ICollection_getSize,
		ArrayList_ICollection_destroy,
		ArrayList_ICollection_add,
		NULL
	};
	
	ICollection* retVal = malloc(sizeof(ICollection));
	memcpy(retVal, &tmpVal, sizeof(ICollection));	

	retVal->subType = ArrayList_IList_create(retVal);

	return retVal;
}

IList* ArrayList_IList_create(ICollection* pCollection)
{
	IList tmpVal = {
		ArrayList_IList_getSize,
		ArrayList_IList_destroy,
		ArrayList_IList_add,
		ArrayList_IList_get,
		ArrayList_IList_remove,
		NULL,
		NULL	
	};

	IList* retVal = malloc(sizeof(IList));
	memcpy(retVal, &tmpVal, sizeof(IList));

	retVal->subType = ArrayList_create(retVal);
	retVal->superType = pCollection;

	return retVal;
}


ArrayList* ArrayList_create(IList* pList)
{
	ArrayList* retVal;
	int* values = (int*)malloc(sizeof(int) * ARRAY_LIST_INITIAL_CAPACITY);
	ArrayList tmpVal = {
		NULL,
		0,
		ARRAY_LIST_INITIAL_CAPACITY,
		NULL
	};
	tmpVal.superType = pList;
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
		0,
		NULL
	};
	values = ArrayList_allocateArray(pOther->values, pOther->size, pOther->capacity);
	tmpVal.values = values;
	tmpVal.capacity = pOther->capacity;
	tmpVal.size = pOther->size;
	memcpy(retVal, &tmpVal, sizeof(ArrayList));

	return retVal;
}

void ArrayList_ICollection_destroy(ICollection* pCollection)
{
	ArrayList_IList_destroy(pCollection->subType);
	free(pCollection->subType);
	free(pCollection);
}

void ArrayList_IList_destroy(IList* pList)
{
	ArrayList_destroy(pList->subType);
	free(pList->subType);	
}

void ArrayList_destroy(ArrayList* pList)
{
	free(pList->values);
}

int ArrayList_ICollection_getSize(const ICollection* pCollection)
{
	return ArrayList_IList_getSize(pCollection->subType);
}

int ArrayList_IList_getSize(const IList* pList)
{
	ArrayList* list = pList->subType;
	return list->size;
}

void ArrayList_ICollection_add(ICollection* pCollection, const int pValue)
{
	ArrayList_IList_add(pCollection->subType, pValue);
}

void ArrayList_IList_add(IList* pList, const int pValue)
{
	ArrayList_add(pList->subType, pValue);
}

void ArrayList_add(ArrayList* pList, const int pValue)
{
	/* Expand the array if we have no more room */
	if (pList->capacity == pList->size)
	{
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

int ArrayList_IList_remove(IList* pList, const size_t pIndex)
{
	return ArrayList_remove(pList->subType, pIndex);
}

int ArrayList_remove(ArrayList* pList, const size_t pIndex)
{
	int removedVal = 0;
	size_t i = 0;
	size_t newCapacity = 0;
	
	if (pIndex >= pList->size)
	{
		/*TODO: Need some sort of return code to prevent this */
	}

	removedVal = pList->values[pIndex];
	
	--pList->size;
	for (i = pIndex; i < pList->size; ++i)
	{
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

int ArrayList_IList_get(const IList* pList, const size_t pIndex)
{
	return ArrayList_get(pList->subType, pIndex);
}

int ArrayList_get(const ArrayList* pList, const size_t pIndex)
{
	int retVal = 0;
	
	if (pIndex > pList->size)
	{
		/* TODO: Return null when doing void ptrs */
		retVal = -1;
	}
	else
	{
		retVal = pList->values[pIndex];
	}

	return retVal;
}

int* ArrayList_allocateArray(
	const int* pOrigValues,
	const size_t pOrigValuesSize,
	const size_t pNewCapacity)
{
	size_t origValuesBytes;
	size_t retValBytes;
	int* retVal;

	if (pOrigValuesSize > pNewCapacity)
	{
		/* TODO: Return error code or something*/
	}

	origValuesBytes = sizeof(int) * pOrigValuesSize;
	retValBytes = sizeof(int) * pNewCapacity;
	retVal = (int*)malloc(retValBytes);
	memcpy(retVal, pOrigValues, origValuesBytes);

	return retVal;
}
