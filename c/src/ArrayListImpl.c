#include "ArrayListImpl.h"

/* Utility Methods */
static int* _ArrayListImpl_allocateArray(
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

/* Public Methods */
ArrayListImpl* ArrayListImpl_createDefault()
{
	return ArrayListImpl_create(10, 2, 4, 2);
}

ArrayListImpl* ArrayListImpl_create(
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor)
{
	ArrayListImpl* retVal = malloc(sizeof(ArrayListImpl));
	
	int* values = (int*)malloc(sizeof(int) * pCapacity);

	/* Safety Dance:
 	 * Ensure that the remove reallocation threshold is NEVER lower
 	 * than the rempove reallocation threshold.
 	 * Set the threshold to double the divisor if this happens.
 	 */
	unsigned int removeReallocationThreshold = pRemoveReallocationThreshold;
	if (removeReallocationThreshold < pRemoveReallocationDivisor)
	{
		removeReallocationThreshold = pRemoveReallocationDivisor * 2;
	}

	retVal->values = values;
	retVal->size = 0;
	retVal->capacity = pCapacity;
	retVal->initialCapacity = pCapacity;
	retVal->values = values;
	retVal->addReallocationMultiplier = pAddReallocationMultiplier;
	retVal->removeReallocationThreshold = removeReallocationThreshold;
	retVal->removeReallocationDivisor = pRemoveReallocationDivisor;
	
	return retVal;
}

void ArrayListImpl_delete(ArrayListImpl* pList)
{
	free(pList->values);
}

void ArrayListImpl_add(ArrayListImpl* pList, const int pValue)
{
	/* Expand the array if we have no more room */
	if (pList->capacity == pList->size)
	{
		size_t newCapacity = pList->capacity * pList->addReallocationMultiplier;
		int* newValues;
		if (newCapacity == 0) { newCapacity++; }  /* Gotta handle 0... */
		newValues = _ArrayListImpl_allocateArray(pList->values, pList->size, newCapacity);
		free(pList->values);
		pList->values = newValues;
		pList->capacity = newCapacity;
	}

	pList->values[pList->size++] = pValue;
}

int ArrayListImpl_remove(ArrayListImpl* pList, const size_t pIndex)
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
	newCapacity = pList->capacity / pList->removeReallocationDivisor;
	if (newCapacity >= pList->initialCapacity
		&& (pList->size * pList->removeReallocationThreshold) < pList->capacity)
	{
		int* newValues = _ArrayListImpl_allocateArray(pList->values, pList->size, newCapacity);
		free(pList->values);
		pList->values = newValues;
		pList->capacity = newCapacity;
	}

	return removedVal;
}

int ArrayListImpl_get(const ArrayListImpl* pList, const size_t pIndex)
{
	int retVal = 0;
	
	if (pIndex >= pList->size)
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

ArrayListImplIterator* ArrayListImpl_iterator(ArrayListImpl* pList)
{
	ArrayListImplIterator* retVal = malloc(sizeof(ArrayListImplIterator));

	retVal->nextIndex = 0;
	retVal->list = pList;

	return retVal;
}

Boolean ArrayListImplIterator_hasNext(ArrayListImplIterator* pIter)
{
	Boolean retVal = FALSE;

	if (pIter->nextIndex < pIter->list->size)
	{
		retVal = TRUE;
	}

	return retVal;
}

int ArrayListImplIterator_peekNext(ArrayListImplIterator* pIter)
{
	int retVal = 0;
	
	if (ArrayListImplIterator_hasNext(pIter))
	{
		retVal = ArrayListImpl_get(
			pIter->list,
			pIter->nextIndex
		);
	}

	return retVal;
}

int ArrayListImplIterator_next(ArrayListImplIterator* pIter)
{
	int retVal = 0;
	
	if (ArrayListImplIterator_hasNext(pIter))
	{
		retVal = ArrayListImpl_get(
			pIter->list, 
			pIter->nextIndex
		);
		++pIter->nextIndex;
	}

	return retVal;
}

void ArrayListImplIterator_delete(ArrayListImplIterator* pIter)
{
	/* Do NOT free the list pointer within the iterator!
	 */
	free(pIter);
}
