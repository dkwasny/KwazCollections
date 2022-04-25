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
    ArrayList* pList,
    const size_t startIndex,
    const size_t endIndex,
    ArrayList* workArray)
{
    const size_t midpoint = (startIndex + endIndex) / 2;

    const size_t leftStart = startIndex;
    const size_t leftEnd = midpoint;
    size_t leftIndex = leftStart;

    const size_t rightStart = midpoint + 1;
    const size_t rightEnd = endIndex;
    size_t rightIndex = rightStart;

    Boolean leftHasData = leftIndex <= leftEnd;
    Boolean rightHasData = rightIndex <= rightEnd;
    size_t i = 0;

    /* Terminal condition */
    if (startIndex >= endIndex)
    {
        return;
    }

    _ArrayList_mergeSortSublist(pList, leftStart, leftEnd, workArray);
    _ArrayList_mergeSortSublist(pList, rightStart, rightEnd, workArray);

    workArray->size = 0;

    while (leftHasData || rightHasData)
    {
        if (leftHasData && !rightHasData)
        {
            ArrayList_add(
                workArray,
                ArrayList_get(pList, leftIndex++)
            );
        }
        else if (!leftHasData)
        {
            ArrayList_add(
                workArray,
                ArrayList_get(pList, rightIndex++)
            );
        }
        else
        {
            int firstElement = ArrayList_get(pList, leftIndex);
            int secondElement = ArrayList_get(pList, rightIndex);
            if (firstElement < secondElement)
            {
                ArrayList_add(workArray, firstElement);
                ++leftIndex;
            }
            else if (firstElement > secondElement)
            {
                ArrayList_add(workArray, secondElement);
                ++rightIndex;
            }
            else
            {
                ArrayList_add(workArray, firstElement);
                ++leftIndex;
                ArrayList_add(workArray, secondElement);
                ++rightIndex;
            }
        }

        leftHasData = leftIndex <= leftEnd;
        rightHasData = rightIndex <= rightEnd;
    }

    for (i = 0; i < workArray->size; ++i)
    {
        ArrayList_set(
            pList,
            ArrayList_get(workArray, i),
            i + startIndex
        );
    }
}

ArrayList* ArrayList_mergeSort(ArrayList* pList)
{
    if (pList->size > 1U)
    {
        ArrayList* workArray = ArrayList_createWithCapacity(pList->size);
        _ArrayList_mergeSortSublist(pList, 0, pList->size - 1, workArray);
        ArrayList_destroy(workArray);
    }
    return pList;
}

static void _ArrayList_quickSortSublist(
    ArrayList* pList,
    const size_t start,
    const size_t end)
{
    size_t lo = start;
    size_t hi = end - 1;
    size_t pivot = end;

    const size_t leftStart = start;
    size_t leftEnd = 0;

    size_t rightStart = 0;
    const size_t rightEnd = end;

    const int pivotValue = ArrayList_get(pList, pivot);

    if (start >= end)
    {
        return;
    }

    while (lo < hi)
    {
        while (lo < hi && ArrayList_get(pList, lo) <= pivotValue)
        {
            lo++;
        }

        while (lo < hi && ArrayList_get(pList, hi) >= pivotValue)
        {
            hi--;
        }

        if (lo < hi)
        {
            _ArrayList_swapElements(pList, lo, hi);
        }
    }

    /*
     * If `lo` ends up next to the original `hi` value (end - 1),
     * then we need to see if we need to swap `lo` and `pivot`.
     *
     * If `lo` is in any other position, we can assume a swap
     * is needed.
     */
    if (lo < end - 1 || ArrayList_get(pList, lo) > pivotValue)
    {
        _ArrayList_swapElements(pList, lo, pivot);
        pivot = lo;
    }

    /*
     * Prevent `pivot` from underflowing.
     * I need to work through the recurse constraints a bit more.
     * I'm missing something here.
     */
    if (pivot != 0)
    {
        leftEnd = pivot - 1;
        _ArrayList_quickSortSublist(pList, leftStart, leftEnd);
    }

    rightStart = pivot + 1;
    _ArrayList_quickSortSublist(pList, rightStart, rightEnd);
}

ArrayList* ArrayList_quickSort(ArrayList* pList)
{
    if (pList->size > 1U)
    {
        _ArrayList_quickSortSublist(pList, 0, pList->size - 1);
    }
    return pList;
}
