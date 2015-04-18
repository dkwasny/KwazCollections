#include "ArrayList.h"
#include "Boolean.h" /* Some internal methods use Boolean */

/* Destroy Methods
 *
 * The main way to fully destroy an ArrayList and its OO model is to always
 * start the destruction at the highest (currently ICollection) level
 * and work your way down.
 * 
 * That is why there are "destroyDelegate" methods for the lower level
 * interfaces.  They delegate to the highest (ICollection) destroy
 * method and start the downward chain of destruction.
 */
static void _ArrayList_ArrayList_destroy(ArrayList* pList)
{
	ArrayListImpl_destroy(pList->impl);
	free(pList);
}

static void _ArrayList_IList_destroy(IList* pList)
{
	_ArrayList_ArrayList_destroy(pList->subType);
	free(pList);
}

static void _ArrayList_ICollection_destroy(ICollection* pCollection)
{
	_ArrayList_IList_destroy(pCollection->subType);
	free(pCollection);
}

static void _ArrayList_ArrayList_destroyDelegate(ArrayList* pList)
{
	_ArrayList_ICollection_destroy(pList->superType->superType);
}

static void _ArrayList_IList_destroyDelegate(IList* pList)
{
	_ArrayList_ICollection_destroy(pList->superType);
}

/* GetSize Methods */
static size_t _ArrayList_ArrayList_getSize(const ArrayList* pList)
{
	return pList->impl->size;
}

static size_t _ArrayList_IList_getSize(const IList* pList)
{
	return _ArrayList_ArrayList_getSize(pList->subType);
}

static size_t _ArrayList_ICollection_getSize(const ICollection* pCollection)
{
	return _ArrayList_IList_getSize(pCollection->subType);
}

/* Add Methods */
static void _ArrayList_ArrayList_add(ArrayList* pList, const void* pValue)
{
	ArrayListImpl_add(pList->impl, pValue);
}

static void _ArrayList_IList_add(IList* pList, const void* pValue)
{
	_ArrayList_ArrayList_add(pList->subType, pValue);
}

static void _ArrayList_ICollection_add(ICollection* pCollection, const void* pValue)
{
	_ArrayList_IList_add(pCollection->subType, pValue);
}

/* AddAll Methods */
static void _ArrayList_IList_addAll(IList* pList, ICollection* pOtherCollection)
{
	ICollection_addAll(pList->superType, pOtherCollection);
}

static void _ArrayList_ArrayList_addAll(ArrayList* pList, ICollection* pOtherCollection)
{
	_ArrayList_IList_addAll(pList->superType, pOtherCollection);
}

/* Contains Methods */
static Boolean _ArrayList_ArrayList_contains(ArrayList* pList, const void* pValue)
{
	return ArrayListImpl_contains(pList->impl, pValue);
}

static Boolean _ArrayList_IList_contains(IList* pList, const void* pValue)
{
	return _ArrayList_ArrayList_contains(pList->subType, pValue);
}

static Boolean _ArrayList_ICollection_contains(ICollection* pList, const void* pValue)
{
	return _ArrayList_IList_contains(pList->subType, pValue);
}

/* Remove Methods */
static void _ArrayList_ArrayList_remove(ArrayList* pList, const size_t pIndex)
{
	ArrayListImpl_remove(pList->impl, pIndex);
}

static void _ArrayList_IList_remove(IList* pList, const size_t pIndex)
{
	_ArrayList_ArrayList_remove(pList->subType, pIndex);
}

/* Get Methods */
static void* _ArrayList_ArrayList_get(const ArrayList* pList, const size_t pIndex)
{
	return ArrayListImpl_get(pList->impl, pIndex);
}

static void* _ArrayList_IList_get(const IList* pList, const size_t pIndex)
{
	return _ArrayList_ArrayList_get(pList->subType, pIndex);
}

/* GetCapacity Methods */
static size_t _ArrayList_ArrayList_getCapacity(const ArrayList* pList)
{
	return pList->impl->capacity;
}

/* Iterator Methods */
static Boolean _ArrayList_IIterator_hasNext(IIterator* pIter)
{
	return ArrayListImplIterator_hasNext(pIter->impl);
}

static void* _ArrayList_IIterator_peekNext(IIterator* pIter)
{
	return ArrayListImplIterator_peekNext(pIter->impl);
}

static void* _ArrayList_IIterator_next(IIterator* pIter)
{
	return ArrayListImplIterator_next(pIter->impl);
}

static void _ArrayList_IIterator_remove(IIterator* pIter)
{
	ArrayListImplIterator_remove(pIter->impl);
}

static void _ArrayList_IIterator_destroy(IIterator* pIter)
{
	ArrayListImplIterator_destroy(pIter->impl);
	free(pIter);
}

/* Iterator Creation Methods */
static IIterator* _ArrayList_ArrayList_iterator(ArrayList* pList)
{
	IIterator tmpVal = {
		_ArrayList_IIterator_hasNext,
		_ArrayList_IIterator_peekNext,
		_ArrayList_IIterator_next,
		_ArrayList_IIterator_remove,
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
		ICollection_addAll,
		_ArrayList_ICollection_contains,
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
		_ArrayList_IList_destroyDelegate,
		_ArrayList_IList_add,
		_ArrayList_IList_addAll,
		_ArrayList_IList_contains,
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
		_ArrayList_ArrayList_destroyDelegate,
		_ArrayList_ArrayList_add,
		_ArrayList_ArrayList_addAll,
		_ArrayList_ArrayList_contains,
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
ArrayList* ArrayList_createDefault(const size_t pTypeSize)
{
	return _ArrayList_createInternal(
		ArrayListImpl_createDefault(pTypeSize)
	);
}

ArrayList* ArrayList_create(
	const size_t pTypeSize,
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor)
{
	return _ArrayList_createInternal(
		ArrayListImpl_create(
			pTypeSize,
			pCapacity,
			pAddReallocationMultiplier,
			pRemoveReallocationThreshold,
			pRemoveReallocationDivisor
		)
	);
}

ArrayList* ArrayList_createCompare(
	const size_t pTypeSize,
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor,
	int (* pCompare)(const void* first, const void* second, size_t size))
{
	return _ArrayList_createInternal(
		ArrayListImpl_createCompare(
			pTypeSize,
			pCapacity,
			pAddReallocationMultiplier,
			pRemoveReallocationThreshold,
			pRemoveReallocationDivisor,
			pCompare
		)
	);
}
