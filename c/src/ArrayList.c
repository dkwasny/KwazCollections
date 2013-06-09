#include "ArrayList.h"

typedef struct _ArrayList
{
	int* values;
	size_t size;
	size_t capacity;

	size_t initialCapacity;
	unsigned int addReallocationMultiplier;
	unsigned int removeReallocationThreshold;
	unsigned int removeReallocationDivisor;
} _ArrayList;

/* Utility methods */
static int* _ArrayList_allocateArray(
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

/* Destructor Methods
 *
 * The main way to fully destroy an ArrayList and its OO model is to always
 * start the destruction at the highest (currently ICollection) level
 * and work your way down.
 * 
 * That is why there are "delete" and "destroy" methods.
 * Calling a destroy method should free the entire OO stack
 * while a delete should only free the memory for that portion
 * of the OO stack. 
 */
static void _ArrayList_delete(_ArrayList* pList)
{
	free(pList->values);
}

static void _ArrayList_ArrayList_delete(ArrayList* pList)
{
	_ArrayList_delete(pList->impl);
	free(pList->impl);
}

static void _ArrayList_IList_delete(IList* pList)
{
	_ArrayList_ArrayList_delete(pList->subType);
	free(pList->subType);	
}

static void _ArrayList_ICollection_destroy(ICollection* pCollection)
{
	_ArrayList_IList_delete(pCollection->subType);
	free(pCollection->subType);
	free(pCollection);
}

static void _ArrayList_ArrayList_destroy(ArrayList* pList)
{
	_ArrayList_ICollection_destroy(pList->superType->superType);
}

static void _ArrayList_IList_destroy(IList* pList)
{
	_ArrayList_ICollection_destroy(pList->superType);
}

/* GetSize Methods */
static int _ArrayList_ArrayList_getSize(const ArrayList* pList)
{
	return pList->impl->size;
}

static int _ArrayList_IList_getSize(const IList* pList)
{
	return _ArrayList_ArrayList_getSize(pList->subType);
}

int _ArrayList_ICollection_getSize(const ICollection* pCollection)
{
	return _ArrayList_IList_getSize(pCollection->subType);
}

/* Add Methods */
static void _ArrayList_add(_ArrayList* pList, const int pValue)
{
	/* Expand the array if we have no more room */
	if (pList->capacity == pList->size)
	{
		size_t newCapacity = pList->capacity * pList->addReallocationMultiplier;
		int* newValues;
		if (newCapacity == 0) { newCapacity++; }  /* Gotta handle 0... */
		newValues = _ArrayList_allocateArray(pList->values, pList->size, newCapacity);
		free(pList->values);
		pList->values = newValues;
		pList->capacity = newCapacity;
	}

	pList->values[pList->size++] = pValue;
}

static void _ArrayList_ArrayList_add(ArrayList* pList, const int pValue)
{
	_ArrayList_add(pList->impl, pValue);
}

static void _ArrayList_IList_add(IList* pList, const int pValue)
{
	_ArrayList_ArrayList_add(pList->subType, pValue);
}

static void _ArrayList_ICollection_add(ICollection* pCollection, const int pValue)
{
	_ArrayList_IList_add(pCollection->subType, pValue);
}

/* Remove Methods */
static int _ArrayList_remove(_ArrayList* pList, const size_t pIndex)
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
		int* newValues = _ArrayList_allocateArray(pList->values, pList->size, newCapacity);
		free(pList->values);
		pList->values = newValues;
		pList->capacity = newCapacity;
	}

	return removedVal;
}

static int _ArrayList_ArrayList_remove(ArrayList* pList, const size_t pIndex)
{
	return _ArrayList_remove(pList->impl, pIndex);
}

static int _ArrayList_IList_remove(IList* pList, const size_t pIndex)
{
	return _ArrayList_ArrayList_remove(pList->subType, pIndex);
}

/* Get Methods */
static int _ArrayList_get(const _ArrayList* pList, const size_t pIndex)
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

static int _ArrayList_ArrayList_get(const ArrayList* pList, const size_t pIndex)
{
	return _ArrayList_get(pList->impl, pIndex);
}

static int _ArrayList_IList_get(const IList* pList, const size_t pIndex)
{
	return _ArrayList_ArrayList_get(pList->subType, pIndex);
}

/* GetCapacity Methods */
static int _ArrayList_ArrayList_getCapacity(const ArrayList* pList)
{
	return pList->impl->capacity;
}

/* Private Creation Methods */
static ICollection* _ArrayList_ICollection_create(IList* pSubType)
{
	ICollection tmpVal = {
		_ArrayList_ICollection_getSize,
		_ArrayList_ICollection_destroy,
		_ArrayList_ICollection_add,
		NULL
	};
	
	ICollection* retVal = malloc(sizeof(ICollection));
	memcpy(retVal, &tmpVal, sizeof(ICollection));	

	retVal->subType = pSubType;

	return retVal;
}

static IList* _ArrayList_IList_create(ArrayList* pSubType)
{
	IList tmpVal = {
		_ArrayList_IList_getSize,
		_ArrayList_IList_destroy,
		_ArrayList_IList_add,
		_ArrayList_IList_get,
		_ArrayList_IList_remove,
		NULL,
		NULL
	};

	IList* retVal = malloc(sizeof(IList));
	memcpy(retVal, &tmpVal, sizeof(IList));

	retVal->subType = pSubType;
	retVal->superType = _ArrayList_ICollection_create(retVal);

	return retVal;
}

/* Public methods */
ArrayList* ArrayList_create(
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor)
{
	ArrayList tmpVal = {
		_ArrayList_ArrayList_getSize,
		_ArrayList_ArrayList_destroy,
		_ArrayList_ArrayList_add,
		_ArrayList_ArrayList_get,
		_ArrayList_ArrayList_remove,
		_ArrayList_ArrayList_getCapacity,
		NULL,
		NULL
	};

	ArrayList* retVal = malloc(sizeof(ArrayList));
	_ArrayList* impl = malloc(sizeof(_ArrayList));
	
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

	memcpy(retVal, &tmpVal, sizeof(ArrayList));
	
	impl->values = values;
	impl->size = 0;
	impl->capacity = pCapacity;
	impl->initialCapacity = pCapacity;
	impl->values = values;
	impl->addReallocationMultiplier = pAddReallocationMultiplier;
	impl->removeReallocationThreshold = removeReallocationThreshold;
	impl->removeReallocationDivisor = pRemoveReallocationDivisor;

	retVal->superType = _ArrayList_IList_create(retVal);
	retVal->impl = impl;	

	return retVal;
}

ArrayList* ArrayList_createDefault()
{
	/* See header for reasoning and explanation on the default values
	 */
	return ArrayList_create(10, 2, 4, 2);
}

