#include "IntArrayListImpl.h"

IntArrayListImpl* IntArrayListImpl_create(
	ArrayListImpl* pList)
{
	IntArrayListImpl* retVal = malloc(sizeof(IntArrayListImpl));
	retVal->list = pList;
	return retVal;
}

void IntArrayListImpl_destroy(IntArrayListImpl* pList)
{
	ArrayListImpl_destroy(pList->list);
	free(pList);
}

void IntArrayListImpl_add(IntArrayListImpl* pList, const int pValue)
{
	ArrayListImpl_add(pList->list, &pValue);
}

void IntArrayListImpl_addAll(IntArrayListImpl* pList, const IntArrayListImpl* pOtherList)
{
	ArrayListImpl_addAll(pList->list, pOtherList->list);
}

void IntArrayListImpl_remove(IntArrayListImpl* pList, const size_t pIndex)
{
	ArrayListImpl_remove(pList->list, pIndex);
}

int IntArrayListImpl_get(const IntArrayListImpl* pList, const size_t pIndex)
{
	return *(int*)ArrayListImpl_get(pList->list, pIndex);
}

Boolean IntArrayListImpl_contains(const IntArrayListImpl* pList, const int pValue)
{
	return ArrayListImpl_contains(pList->list, &pValue);
}
