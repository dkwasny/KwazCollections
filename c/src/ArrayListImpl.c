#include "ArrayListImpl.h"

/* Utility Methods */
static void** _ArrayListImpl_allocateArray(
	const void* pOrigValues,
	const size_t pOrigValuesSize,
	const size_t pNewCapacity)
{
	size_t origValuesBytes;
	size_t retValBytes;
	void** retVal;

	if (pOrigValuesSize > pNewCapacity)
	{
		/* TODO: Return error code or something*/
		return NULL;
	}

	origValuesBytes = sizeof(void*) * pOrigValuesSize;
	retValBytes = sizeof(void*) * pNewCapacity;
	retVal = malloc(retValBytes);
	memcpy(retVal, pOrigValues, origValuesBytes);

	return retVal;
}

/* Public Methods */
ArrayListImpl* ArrayListImpl_createDefault(const size_t pTypeSize)
{
	return ArrayListImpl_create(pTypeSize, 10, 2, 4, 2);
}

ArrayListImpl* ArrayListImpl_create(
	const size_t pTypeSize,
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor)
{
	ArrayListImpl* retVal = malloc(sizeof(ArrayListImpl));
	
	void** values = malloc(sizeof(void*) * pCapacity);

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

	retVal->typeSize = pTypeSize;
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

void ArrayListImpl_destroy(ArrayListImpl* pList)
{
	ArrayListImplIterator* iter = ArrayListImpl_iterator(pList);
	while (ArrayListImplIterator_hasNext(iter))
	{
		free(ArrayListImplIterator_next(iter));
	}
	ArrayListImplIterator_destroy(iter);
	free(pList->values);
	free(pList);
}

void ArrayListImpl_add(ArrayListImpl* pList, const void* pValue)
{
	void* copiedValue = NULL;

	/* Expand the array if we have no more room */
	if (pList->capacity == pList->size)
	{
		size_t newCapacity = pList->capacity * pList->addReallocationMultiplier;
		void** newValues;
		if (newCapacity == 0) { newCapacity++; }  /* Gotta handle 0... */
		newValues = _ArrayListImpl_allocateArray(
			pList->values,
			pList->size,
			newCapacity
		);
		free(pList->values);
		pList->values = newValues;
		pList->capacity = newCapacity;
	}

	copiedValue = malloc(pList->typeSize);
	memcpy(copiedValue, pValue, pList->typeSize);
	pList->values[pList->size++] = copiedValue;
}

void ArrayListImpl_remove(ArrayListImpl* pList, const size_t pIndex)
{
	size_t i = 0;
	size_t newCapacity = 0;
	
	if (pIndex >= pList->size)
	{
		/*TODO: Need some sort of return code to prevent this */
		return;
	}

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
		void** newValues = _ArrayListImpl_allocateArray(
			pList->values,
			pList->size,
			newCapacity
		);
		free(pList->values);
		pList->values = newValues;
		pList->capacity = newCapacity;
	}
}

void* ArrayListImpl_get(const ArrayListImpl* pList, const size_t pIndex)
{
	void* retVal = NULL;
	
	if (pIndex < pList->size)
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

void* ArrayListImplIterator_peekNext(ArrayListImplIterator* pIter)
{
	void* retVal = NULL;
	
	if (ArrayListImplIterator_hasNext(pIter))
	{
		retVal = ArrayListImpl_get(
			pIter->list,
			pIter->nextIndex
		);
	}

	return retVal;
}

void* ArrayListImplIterator_next(ArrayListImplIterator* pIter)
{
	void* retVal = NULL;
	
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

void ArrayListImplIterator_destroy(ArrayListImplIterator* pIter)
{
	/* Do NOT free the list pointer within the iterator!
	 */
	free(pIter);
}
