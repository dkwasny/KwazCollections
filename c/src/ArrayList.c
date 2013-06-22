#include "ArrayList.h"
#include "Boolean.h" /* Some internal methods use Boolean */

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
static void _ArrayList_ArrayList_delete(ArrayList* pList)
{
	ArrayListImpl_delete(pList->impl);
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

static int _ArrayList_ICollection_getSize(const ICollection* pCollection)
{
	return _ArrayList_IList_getSize(pCollection->subType);
}

/* Add Methods */
static void _ArrayList_ArrayList_add(ArrayList* pList, const int pValue)
{
	ArrayListImpl_add(pList->impl, pValue);
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
static int _ArrayList_ArrayList_remove(ArrayList* pList, const size_t pIndex)
{
	return ArrayListImpl_remove(pList->impl, pIndex);
}

static int _ArrayList_IList_remove(IList* pList, const size_t pIndex)
{
	return _ArrayList_ArrayList_remove(pList->subType, pIndex);
}

/* Get Methods */
static int _ArrayList_ArrayList_get(const ArrayList* pList, const size_t pIndex)
{
	return ArrayListImpl_get(pList->impl, pIndex);
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

/* Iterator Methods */
static Boolean _ArrayList_IIterator_hasNext(IIterator* pIter)
{
	return ArrayListImplIterator_hasNext(pIter->impl);
}

static int _ArrayList_IIterator_peekNext(IIterator* pIter)
{
	return ArrayListImplIterator_peekNext(pIter->impl);
}

static int _ArrayList_IIterator_next(IIterator* pIter)
{
	return ArrayListImplIterator_next(pIter->impl);
}

static void _ArrayList_IIterator_destroy(IIterator* pIter)
{
	ArrayListImplIterator_delete(pIter->impl);
	free(pIter);
}

/* Iterator Creation Methods */
static IIterator* _ArrayList_ArrayList_iterator(ArrayList* pList)
{
	IIterator tmpVal = {
		_ArrayList_IIterator_hasNext,
		_ArrayList_IIterator_peekNext,
		_ArrayList_IIterator_next,
		_ArrayList_IIterator_destroy,
		NULL
	};

	IIterator* retVal = malloc(sizeof(IIterator));
	memcpy(retVal, &tmpVal, sizeof(IIterator));

	retVal->impl = ArrayListImpl_iterator(pList->impl);

	return retVal;
}

static IIterator* _ArrayList_IList_iterator(IList* pList)
{
	return _ArrayList_ArrayList_iterator(pList->subType);
}

static IIterator* _ArrayList_ICollection_iterator(ICollection* pCollection)
{
	return _ArrayList_IList_iterator(pCollection->subType);
}

/* Private Creation Methods */
static ICollection* _ArrayList_ICollection_create(IList* pSubType)
{
	ICollection tmpVal = {
		_ArrayList_ICollection_getSize,
		_ArrayList_ICollection_destroy,
		_ArrayList_ICollection_add,
		_ArrayList_ICollection_iterator,
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
		_ArrayList_IList_iterator,
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

static ArrayList* _ArrayList_createInternal(ArrayListImpl* pImpl)
{
	ArrayList tmpVal = {
		_ArrayList_ArrayList_getSize,
		_ArrayList_ArrayList_destroy,
		_ArrayList_ArrayList_add,
		_ArrayList_ArrayList_iterator,
		_ArrayList_ArrayList_get,
		_ArrayList_ArrayList_remove,
		_ArrayList_ArrayList_getCapacity,
		NULL,
		NULL
	};

	ArrayList* retVal = malloc(sizeof(ArrayList));
	memcpy(retVal, &tmpVal, sizeof(ArrayList));
	
	retVal->superType = _ArrayList_IList_create(retVal);
	retVal->impl = pImpl;	

	return retVal;
}

/* Public methods */
ArrayList* ArrayList_create(
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor)
{
	return _ArrayList_createInternal(
		ArrayListImpl_create(
			pCapacity,
			pAddReallocationMultiplier,
			pRemoveReallocationThreshold,
			pRemoveReallocationDivisor
		)
	);
}

ArrayList* ArrayList_createDefault()
{
	return _ArrayList_createInternal(
		ArrayListImpl_createDefault()
	);
}
