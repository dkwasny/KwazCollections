#include "ArrayList.h"
#include <string.h>

/* Internal Iterator Struct */
typedef struct ArrayListIterator
{
	size_t nextIndex;
	ArrayList* list;
} ArrayListIterator;

/* Utility Methods */
static void** _ArrayList_allocateArray(
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

static void _ArrayList_expandArray(ArrayList* pList, const size_t newCapacity)
{
	size_t adjustedCapacity = newCapacity;
	void** newValues;
	if (adjustedCapacity == 0) { adjustedCapacity++; }  /* Gotta handle 0... */
	newValues = _ArrayList_allocateArray(
		pList->values,
		pList->size,
		adjustedCapacity
	);
	free(pList->values);
	pList->values = newValues;
	pList->capacity = adjustedCapacity;
}

/* Public Methods */
ArrayList* ArrayList_createDefault(const size_t pTypeSize)
{
	return ArrayList_create(
		pTypeSize,
		memcmp,
		memcpy
	);
}

ArrayList* ArrayList_create(
	const size_t pTypeSize,
	int (* const pCompare)(const void* first, const void* second, size_t size),
	void* (* const pCopy)(void * dest, const void * src, size_t size))
{
	return ArrayList_createFull(
		pTypeSize,
		10,
		2,
		4,
		2,
		pCompare,
		pCopy
	);
}

ArrayList* ArrayList_createFull(
	const size_t pTypeSize,
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor,
	int (* const pCompare)(const void* first, const void* second, size_t size),
	void* (* const pCopy)(void * dest, const void * src, size_t size))
{
	ArrayList* retVal = malloc(sizeof(ArrayList));
	
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

void ArrayList_destroy(ArrayList* pList)
{
	Iterator* iter = ArrayList_iterator(pList);
	while (iter->hasNext(iter))
	{
		free(iter->next(iter));
	}
	iter->destroy(iter);
	free(pList->values);
	free(pList);
}

ArrayList* ArrayList_add(ArrayList* pList, const void* pValue)
{
	void* copiedValue = NULL;

	/* Expand the array if we have no more room */
	if (pList->capacity == pList->size)
	{
		size_t newCapacity = pList->capacity * pList->addReallocationMultiplier;
		_ArrayList_expandArray(pList, newCapacity);
	}

	copiedValue = malloc(pList->typeSize);
	pList->values[pList->size++]
		= pList->copy(copiedValue, pValue, pList->typeSize);
		
	return pList;
}

ArrayList* ArrayList_addAll(ArrayList* pList, const ArrayList* pOtherList)
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
		_ArrayList_expandArray(pList, combinedSize);
	}

	for (i = 0; i < pOtherList->size; ++i)
	{
		ArrayList_add(pList, ArrayList_get(pOtherList, i));	
	}
	
	return pList;
}

ArrayList* ArrayList_addIterator(ArrayList* pList, Iterator* pIter)
{
	while (pIter->hasNext(pIter))
	{
		ArrayList_add(pList, pIter->next(pIter));
	}
	
	return pList;
}

ArrayList* ArrayList_remove(ArrayList* pList, const size_t pIndex)
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
		void** newValues = _ArrayList_allocateArray(
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

void* ArrayList_get(const ArrayList* pList, const size_t pIndex)
{
	void* retVal = NULL;
	
	if (pIndex < pList->size)
	{
		retVal = pList->values[pIndex];
	}

	return retVal;
}

ArrayList* ArrayList_set(ArrayList* pList, const void* value, const size_t pIndex)
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

Boolean ArrayList_contains(const ArrayList* pList, const void* pValue)
{
	Boolean found = FALSE;

	int compare = 0;
	size_t index = 0;
	for (; index < pList->size && found == FALSE; ++index)
	{
		compare = pList->compare(
			ArrayList_get(pList, index),
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

static void _ArrayList_mergeSortSublist(
	ArrayList* pList,
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

	ArrayList* store = NULL;

	/* Terminal condition */
	if (startIndex >= endIndex)
	{
		return;
	}
	
	store = ArrayList_create(
		pList->typeSize,
		pList->compare,
		pList->copy
	);
	
	_ArrayList_mergeSortSublist(pList, firstHalfStart, firstHalfEnd);
	_ArrayList_mergeSortSublist(pList, secondHalfStart, secondHalfEnd);

	while (firstListHasData || secondListHasData)
	{
		if (firstListHasData && !secondListHasData)
		{
			ArrayList_add(
				store,
				ArrayList_get(pList, firstIndex++)
			);
		}
		else if (!firstListHasData)
		{
			ArrayList_add(
				store,
				ArrayList_get(pList, secondIndex++)
			);
		}
		else
		{
			void* firstElement = ArrayList_get(pList, firstIndex);
			void* secondElement = ArrayList_get(pList, secondIndex);
			compare = pList->compare(
				firstElement,
				secondElement,
				pList->typeSize
			);
			if (compare < 0)
			{
				ArrayList_add(store, firstElement);
				++firstIndex;
			}
			else if (compare > 0)
			{
				ArrayList_add(store, secondElement);
				++secondIndex;
			}
			else
			{
				ArrayList_add(store, firstElement);
				++firstIndex;
				ArrayList_add(store, secondElement);
				++secondIndex;
			}
		}

		firstListHasData = firstIndex <= firstHalfEnd;
		secondListHasData = secondIndex <= secondHalfEnd;
	}

	for (i = 0; i < store->size; ++i)
	{
		ArrayList_set(
			pList, 
			ArrayList_get(store, i),
			i + startIndex
		);
	}

	ArrayList_destroy(store);
}

ArrayList* ArrayList_mergeSort(ArrayList* pList)
{
	if(pList->size > 1U)
	{
		_ArrayList_mergeSortSublist(pList, 0, pList->size - 1);
	}
	return pList;
}

/* Internal Iterator Methods */
static Boolean _ArrayListIterator_hasNext(const Iterator* pIter)
{
	Boolean retVal = FALSE;
	ArrayListIterator* iter = pIter->impl;
	
	if (iter->nextIndex < iter->list->size)
	{
		retVal = TRUE;
	}

	return retVal;
}

static void* _ArrayListIterator_peekNext(const Iterator* pIter)
{
	void* retVal = NULL;
	ArrayListIterator* iter = pIter->impl;
	
	if (_ArrayListIterator_hasNext(pIter))
	{
		retVal = ArrayList_get(
			iter->list,
			iter->nextIndex
		);
	}

	return retVal;
}

static void* _ArrayListIterator_next(Iterator* pIter)
{
	void* retVal = NULL;
	ArrayListIterator* iter = pIter->impl;
	
	if (_ArrayListIterator_hasNext(pIter))
	{
		retVal = ArrayList_get(
			iter->list, 
			iter->nextIndex
		);
		++iter->nextIndex;
	}

	return retVal;
}

static void _ArrayListIterator_remove(Iterator* pIter)
{
	ArrayListIterator* iter = pIter->impl;

	if (iter->nextIndex > 0)
	{
		size_t removeIndex = iter->nextIndex - 1;
		ArrayList_remove(iter->list, removeIndex);
		--iter->nextIndex;
	}
	/* TODO: Report a bad remove attempt if condition fails */
}

static void _ArrayListIterator_destroy(Iterator* pIter)
{
	/* Do NOT free the list pointer within the iterator!
	 */
	free(pIter->impl);
	free(pIter);
}

Iterator* ArrayList_iterator(ArrayList* pList)
{
	Iterator* retVal = malloc(sizeof(Iterator));
	
	Iterator tmpVal = {
		_ArrayListIterator_hasNext,
		_ArrayListIterator_peekNext,
		_ArrayListIterator_next,
		_ArrayListIterator_remove,
		_ArrayListIterator_destroy,
		NULL
	};

	ArrayListIterator* iter = malloc(sizeof(ArrayListIterator));	
	iter->nextIndex = 0;
	iter->list = pList;
	
	memcpy(retVal, &tmpVal, sizeof(Iterator));

	retVal->impl = iter;
	
	return retVal;
}
