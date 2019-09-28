#include "GenericArrayList.h"
#include <string.h>

/* Internal Iterator Struct */
typedef struct GenericArrayListIterator
{
	size_t nextIndex;
	GenericArrayList* list;
} GenericArrayListIterator;

/* Utility Methods */
static void** _GenericArrayList_allocateArray(
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

static void _GenericArrayList_expandArray(GenericArrayList* pList, const size_t newCapacity)
{
	size_t adjustedCapacity = newCapacity;
	void** newValues;
	if (adjustedCapacity == 0) { adjustedCapacity++; }  /* Gotta handle 0... */
	newValues = _GenericArrayList_allocateArray(
		pList->values,
		pList->size,
		adjustedCapacity
	);
	free(pList->values);
	pList->values = newValues;
	pList->capacity = adjustedCapacity;
}

/* Public Methods */
GenericArrayList* GenericArrayList_createDefault(const size_t pTypeSize)
{
	return GenericArrayList_create(
		pTypeSize,
		memcmp,
		memcpy
	);
}

GenericArrayList* GenericArrayList_create(
	const size_t pTypeSize,
	int (* const pCompare)(const void* first, const void* second, size_t size),
	void* (* const pCopy)(void * dest, const void * src, size_t size))
{
	return GenericArrayList_createFull(
		pTypeSize,
		10,
		2,
		4,
		2,
		pCompare,
		pCopy
	);
}

GenericArrayList* GenericArrayList_createFull(
	const size_t pTypeSize,
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor,
	int (* const pCompare)(const void* first, const void* second, size_t size),
	void* (* const pCopy)(void * dest, const void * src, size_t size))
{
	GenericArrayList* retVal = malloc(sizeof(GenericArrayList));
	
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
	retVal->compare = pCompare;
	retVal->copy = pCopy;

	return retVal;
}

void GenericArrayList_destroy(GenericArrayList* pList)
{
	GenericIterator* iter = GenericArrayList_iterator(pList);
	while (iter->hasNext(iter))
	{
		free(iter->next(iter));
	}
	iter->destroy(iter);
	free(pList->values);
	free(pList);
}

GenericArrayList* GenericArrayList_add(GenericArrayList* pList, const void* pValue)
{
	void* copiedValue = NULL;

	/* Expand the array if we have no more room */
	if (pList->capacity == pList->size)
	{
		size_t newCapacity = pList->capacity * pList->addReallocationMultiplier;
		_GenericArrayList_expandArray(pList, newCapacity);
	}

	copiedValue = malloc(pList->typeSize);
	pList->values[pList->size++]
		= pList->copy(copiedValue, pValue, pList->typeSize);
		
	return pList;
}

GenericArrayList* GenericArrayList_addAll(GenericArrayList* pList, const GenericArrayList* pOtherList)
{
	size_t availableSpots = pList->capacity - pList->size;
	size_t i = 0;
	
	/* Explode if user attempts to add a list to itself. */
	if (pList == pOtherList)
	{
		return NULL;
	}

	if (pOtherList->size > availableSpots)
	{
		size_t combinedSize = pList->size + pOtherList->size;
		_GenericArrayList_expandArray(pList, combinedSize);
	}

	for (i = 0; i < pOtherList->size; ++i)
	{
		GenericArrayList_add(pList, GenericArrayList_get(pOtherList, i));	
	}
	
	return pList;
}

GenericArrayList* GenericArrayList_addIterator(GenericArrayList* pList, GenericIterator* pIter)
{
	while (pIter->hasNext(pIter))
	{
		GenericArrayList_add(pList, pIter->next(pIter));
	}
	
	return pList;
}

GenericArrayList* GenericArrayList_remove(GenericArrayList* pList, const size_t pIndex)
{
	size_t i = 0;
	size_t newCapacity = 0;
	
	if (pIndex >= pList->size)
	{
		/*TODO: Need some sort of return code to prevent this */
		return NULL;
	}
		
	free(pList->values[pIndex]);

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
		void** newValues = _GenericArrayList_allocateArray(
			pList->values,
			pList->size,
			newCapacity
		);
		free(pList->values);
		pList->values = newValues;
		pList->capacity = newCapacity;
	}
	
	return pList;
}

void* GenericArrayList_get(const GenericArrayList* pList, const size_t pIndex)
{
	void* retVal = NULL;
	
	if (pIndex < pList->size)
	{
		retVal = pList->values[pIndex];
	}

	return retVal;
}

GenericArrayList* GenericArrayList_set(GenericArrayList* pList, const void* value, const size_t pIndex)
{
	if (pIndex < pList->size)
	{
		void* newValue = malloc(pList->typeSize);
		free(pList->values[pIndex]);
		pList->values[pIndex] = pList->copy(
			newValue,
			value,
			pList->typeSize
		);
	}

	return pList;
}

Boolean GenericArrayList_contains(const GenericArrayList* pList, const void* pValue)
{
	Boolean found = FALSE;

	int compare = 0;
	size_t index = 0;
	for (; index < pList->size && found == FALSE; ++index)
	{
		compare = pList->compare(
			GenericArrayList_get(pList, index),
			pValue,
			pList->typeSize
		);

		if (compare == 0)
		{
			found = TRUE;
		}	
	}

	return found;
}

static void _GenericArrayList_mergeSortSublist(
	GenericArrayList* pList,
	const size_t startIndex,
	const size_t endIndex)
{
	const size_t size = endIndex - startIndex + 1;
	const size_t halfSize = size / 2;
	const size_t firstHalfStart = startIndex;
	const size_t firstHalfEnd = startIndex + halfSize - 1;
	const size_t secondHalfStart = firstHalfEnd + 1;
	const size_t secondHalfEnd = endIndex;
	size_t firstIndex = firstHalfStart;
	size_t secondIndex = secondHalfStart;

	Boolean firstListHasData = firstIndex <= firstHalfEnd;
	Boolean secondListHasData = secondIndex <= secondHalfEnd;
	int compare = 0;
	size_t i = 0;

	GenericArrayList* store = NULL;

	/* Terminal condition */
	if (startIndex >= endIndex)
	{
		return;
	}

	_GenericArrayList_mergeSortSublist(pList, firstHalfStart, firstHalfEnd);
	_GenericArrayList_mergeSortSublist(pList, secondHalfStart, secondHalfEnd);

	store = GenericArrayList_create(
		pList->typeSize,
		pList->compare,
		pList->copy
	);

	while (firstListHasData || secondListHasData)
	{
		if (firstListHasData && !secondListHasData)
		{
			GenericArrayList_add(
				store,
				GenericArrayList_get(pList, firstIndex++)
			);
		}
		else if (!firstListHasData)
		{
			GenericArrayList_add(
				store,
				GenericArrayList_get(pList, secondIndex++)
			);
		}
		else
		{
			void* firstElement = GenericArrayList_get(pList, firstIndex);
			void* secondElement = GenericArrayList_get(pList, secondIndex);
			compare = pList->compare(
				firstElement,
				secondElement,
				pList->typeSize
			);
			if (compare < 0)
			{
				GenericArrayList_add(store, firstElement);
				++firstIndex;
			}
			else if (compare > 0)
			{
				GenericArrayList_add(store, secondElement);
				++secondIndex;
			}
			else
			{
				GenericArrayList_add(store, firstElement);
				++firstIndex;
				GenericArrayList_add(store, secondElement);
				++secondIndex;
			}
		}

		firstListHasData = firstIndex <= firstHalfEnd;
		secondListHasData = secondIndex <= secondHalfEnd;
	}

	for (i = 0; i < store->size; ++i)
	{
		GenericArrayList_set(
			pList, 
			GenericArrayList_get(store, i),
			i + startIndex
		);
	}

	GenericArrayList_destroy(store);
}

GenericArrayList* GenericArrayList_mergeSort(GenericArrayList* pList)
{
	if(pList->size > 1U)
	{
		_GenericArrayList_mergeSortSublist(pList, 0, pList->size - 1);
	}
	return pList;
}

/* Internal Iterator Methods */
static Boolean _GenericArrayListIterator_hasNext(const GenericIterator* pIter)
{
	Boolean retVal = FALSE;
	GenericArrayListIterator* iter = pIter->impl;
	
	if (iter->nextIndex < iter->list->size)
	{
		retVal = TRUE;
	}

	return retVal;
}

static void* _GenericArrayListIterator_peekNext(const GenericIterator* pIter)
{
	void* retVal = NULL;
	GenericArrayListIterator* iter = pIter->impl;
	
	if (_GenericArrayListIterator_hasNext(pIter))
	{
		retVal = GenericArrayList_get(
			iter->list,
			iter->nextIndex
		);
	}

	return retVal;
}

static void* _GenericArrayListIterator_next(GenericIterator* pIter)
{
	void* retVal = NULL;
	GenericArrayListIterator* iter = pIter->impl;
	
	if (_GenericArrayListIterator_hasNext(pIter))
	{
		retVal = GenericArrayList_get(
			iter->list, 
			iter->nextIndex
		);
		++iter->nextIndex;
	}

	return retVal;
}

static void _GenericArrayListIterator_remove(GenericIterator* pIter)
{
	GenericArrayListIterator* iter = pIter->impl;

	if (iter->nextIndex > 0)
	{
		size_t removeIndex = iter->nextIndex - 1;
		GenericArrayList_remove(iter->list, removeIndex);
		--iter->nextIndex;
	}
	/* TODO: Report a bad remove attempt if condition fails */
}

static void _GenericArrayListIterator_destroy(GenericIterator* pIter)
{
	/* Do NOT free the list pointer within the iterator!
	 */
	free(pIter->impl);
	free(pIter);
}

GenericIterator* GenericArrayList_iterator(GenericArrayList* pList)
{
	GenericIterator* retVal = malloc(sizeof(GenericIterator));
	
	GenericIterator tmpVal = {
		_GenericArrayListIterator_hasNext,
		_GenericArrayListIterator_peekNext,
		_GenericArrayListIterator_next,
		_GenericArrayListIterator_remove,
		_GenericArrayListIterator_destroy,
		NULL
	};

	GenericArrayListIterator* iter = malloc(sizeof(GenericArrayListIterator));	
	iter->nextIndex = 0;
	iter->list = pList;
	
	memcpy(retVal, &tmpVal, sizeof(GenericIterator));

	retVal->impl = iter;
	
	return retVal;
}
