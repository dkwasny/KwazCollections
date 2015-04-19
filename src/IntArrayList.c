#include "IntArrayList.h"

IntArrayList* IntArrayList_create(
	ArrayList* pList)
{
	IntArrayList* retVal = malloc(sizeof(IntArrayList));
	retVal->list = pList;
	return retVal;
}

void IntArrayList_destroy(IntArrayList* pList)
{
	ArrayList_destroy(pList->list);
	free(pList);
}

void IntArrayList_add(IntArrayList* pList, const int pValue)
{
	ArrayList_add(pList->list, &pValue);
}

void IntArrayList_addAll(IntArrayList* pList, const IntArrayList* pOtherList)
{
	ArrayList_addAll(pList->list, pOtherList->list);
}

void IntArrayList_remove(IntArrayList* pList, const size_t pIndex)
{
	ArrayList_remove(pList->list, pIndex);
}

int IntArrayList_get(const IntArrayList* pList, const size_t pIndex)
{
	return *(int*)ArrayList_get(pList->list, pIndex);
}

Boolean IntArrayList_contains(const IntArrayList* pList, const int pValue)
{
	return ArrayList_contains(pList->list, &pValue);
}
