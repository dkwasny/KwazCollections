#include "ArrayList.h"
#include <string.h>
#include <stdio.h>

static const size_t DEFAULT_CAPACITY = 10;
static const unsigned int DEFAULT_ADD_REALLOC_MULT = 2;
static const unsigned int DEFAULT_REM_REALLOC_THRESH = 4;
static const unsigned int DEFAULT_REM_REALLOC_DIV = 2;

/* Utility Methods */
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
        return NULL;
    }

    origValuesBytes = sizeof(int) * pOrigValuesSize;
    retValBytes = sizeof(int) * pNewCapacity;
    retVal = malloc(retValBytes);
    memcpy(retVal, pOrigValues, origValuesBytes);

    return retVal;
}

static void _ArrayList_expandArray(ArrayList* pList, const size_t newCapacity)
{
    size_t adjustedCapacity = newCapacity;
    int* newValues;
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

static void _ArrayList_swapElements(
    ArrayList* pList,
    const size_t first,
    const size_t second)
{
    int firstValue = ArrayList_get(pList, first);
    int secondValue = ArrayList_get(pList, second);

    ArrayList_set(pList, firstValue, second);
    ArrayList_set(pList, secondValue, first);
}

/* Public Methods */
ArrayList* ArrayList_create(void)
{
    return ArrayList_createWithCapacity(
        DEFAULT_CAPACITY
    );
}

ArrayList* ArrayList_createWithCapacity(const size_t pCapacity)
{
    return ArrayList_createFull(
        pCapacity,
        DEFAULT_ADD_REALLOC_MULT,
        DEFAULT_REM_REALLOC_THRESH,
        DEFAULT_REM_REALLOC_DIV
    );
}

ArrayList* ArrayList_createFull(
    const size_t pCapacity,
    const unsigned int pAddReallocationMultiplier,
    const unsigned int pRemoveReallocationThreshold,
    const unsigned int pRemoveReallocationDivisor)
{
    ArrayList* retVal = malloc(sizeof(ArrayList));

    int* values = malloc(sizeof(int) * pCapacity);

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
    retVal->size = 0U;
    retVal->capacity = pCapacity;
    retVal->initialCapacity = pCapacity;
    retVal->values = values;
    retVal->addReallocationMultiplier = pAddReallocationMultiplier;
    retVal->removeReallocationThreshold = removeReallocationThreshold;
    retVal->removeReallocationDivisor = pRemoveReallocationDivisor;

    return retVal;
}

ArrayList* ArrayList_createFromArray(const int* values, const size_t size)
{
    ArrayList* retVal = ArrayList_createFull(
        size,
        DEFAULT_ADD_REALLOC_MULT,
        DEFAULT_REM_REALLOC_THRESH,
        DEFAULT_REM_REALLOC_DIV
    );
    size_t i = 0;
    for (; i < size; i++)
    {
        ArrayList_add(retVal, values[i]);
    }
    return retVal;
}

void ArrayList_destroy(ArrayList* pList)
{
    free(pList->values);
    free(pList);
}

ArrayList* ArrayList_add(ArrayList* pList, const int pValue)
{
    /* Expand the array if we have no more room */
    if (pList->capacity == pList->size)
    {
        size_t newCapacity = pList->capacity * pList->addReallocationMultiplier;
        _ArrayList_expandArray(pList, newCapacity);
    }

    pList->values[pList->size++] = pValue;

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

ArrayList* ArrayList_remove(ArrayList* pList, const size_t pIndex)
{
    size_t i = 0;
    size_t newCapacity = 0;

    if (pIndex >= pList->size)
    {
        /*TODO: Need some sort of return code to prevent this */
        return NULL;
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
        int* newValues = _ArrayList_allocateArray(
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

int ArrayList_get(const ArrayList* pList, const size_t pIndex)
{
    return pList->values[pIndex];
}

ArrayList* ArrayList_set(ArrayList* pList, const int value, const size_t pIndex)
{
    if (pIndex < pList->size)
    {
        pList->values[pIndex] = value;
    }

    return pList;
}

Boolean ArrayList_contains(const ArrayList* pList, const int pValue)
{
    Boolean found = FALSE;

    size_t index = 0;
    for (; index < pList->size && found == FALSE; ++index)
    {
        if (ArrayList_get(pList, index) == pValue)
        {
            found = TRUE;
        }
    }

    return found;
}

static void _ArrayList_mergeSortSublist(
    ArrayList* sourceList,
    ArrayList* destList,
    const size_t startIndex,
    const size_t endIndex)
{
    const size_t midpoint = (startIndex + endIndex) / 2;

    const size_t leftStart = startIndex;
    const size_t leftEnd = midpoint;
    size_t leftIndex = leftStart;

    const size_t rightStart = midpoint + 1;
    const size_t rightEnd = endIndex;
    size_t rightIndex = rightStart;

    size_t i = 0;

    /* Terminal condition */
    if (startIndex >= endIndex)
    {
        return;
    }

    /*
     * Recurse while swapping the role of sourceList and destList.
     * This allows us to keep the in-progress sorted state without requiring
     * to copy data from destList back to sourceList at every level of recursion.
     */
    _ArrayList_mergeSortSublist(destList, sourceList, leftStart, leftEnd);
    _ArrayList_mergeSortSublist(destList, sourceList, rightStart, rightEnd);

    for (i = startIndex; i <= endIndex; i++)
    {
        Boolean leftHasData = leftIndex <= leftEnd;
        Boolean rightHasData = rightIndex <= rightEnd;

        int leftValue = (leftHasData) ? ArrayList_get(sourceList, leftIndex) : -1;
        int rightValue = (rightHasData) ? ArrayList_get(sourceList, rightIndex) : -1;

        if (leftHasData && (!rightHasData || leftValue <= rightValue))
        {
            ArrayList_set(destList, leftValue, i);
            leftIndex++;
        }
        else if (rightHasData)
        {
            ArrayList_set(destList, rightValue, i);
            rightIndex++;
        }
    }
}

ArrayList* ArrayList_mergeSort(ArrayList* pList)
{
    if (pList->size > 1U)
    {
        ArrayList* workArray = ArrayList_createWithCapacity(pList->size);
        ArrayList_addAll(workArray, pList);
        _ArrayList_mergeSortSublist(workArray, pList, 0, pList->size - 1);
        ArrayList_destroy(workArray);
    }
    return pList;
}

static void _ArrayList_quickSortSublist(
    ArrayList* pList,
    const size_t start,
    const size_t end)
{
    int pivotVal = ArrayList_get(pList, start);
    size_t lo = start - 1;
    size_t hi = end + 1;

    if (start >= end)
    {
        return;
    }

    while (1)
    {
        do
        {
            lo++;
        }
        while (ArrayList_get(pList, lo) < pivotVal);

        do
        {
            hi--;
        }
        while(ArrayList_get(pList, hi) > pivotVal);

        if (lo >= hi)
        {
            break;
        }

        _ArrayList_swapElements(pList, lo, hi);
    }

    _ArrayList_quickSortSublist(pList, start, hi);
    _ArrayList_quickSortSublist(pList, hi+1, end);
}

ArrayList* ArrayList_quickSort(ArrayList* pList)
{
    if (pList->size > 1U)
    {
        _ArrayList_quickSortSublist(pList, 0, pList->size - 1);
    }
    return pList;
}
