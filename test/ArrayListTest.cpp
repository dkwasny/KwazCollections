#include "gtest/gtest.h"
#include "ArrayList.h"
#include "Boolean.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static const size_t ARRAY_LIST_RANDOM_SIZE = 5000U;
static const int ARRAY_LIST_MAX_RANDOM_VALUE = 1000;
static const int NUM_RANDOM_SORT_RUNS = 100;

static void ArrayListTest_checkContents(ArrayList* list, size_t offset)
{
    for(size_t i = 0; i < list->size; ++i)
    {
        int expected = int(offset + i);
        int actual = ArrayList_get(list, i);
        ASSERT_EQ(expected, actual);
    }
}

static void ArrayListTest_checkContents(ArrayList* list)
{
    ArrayListTest_checkContents(list, 0);
}

static void ArrayListTest_smokeTest(ArrayList* list)
{
    for (size_t i = 0; i < 20; ++i)
    {
        ArrayList_add(list, i);
    }
    ArrayListTest_checkContents(list);
    for (size_t i = 0; i < 20; ++i)
    {
        ArrayList_remove(list, 0);
    }
}

static void ArrayListTest_compareToArray(ArrayList* list, int* array)
{
    size_t i = 0;
    for (; i < list->size; i++)
    {
        int expected = array[i];
        int actual = ArrayList_get(list, i);
        ASSERT_EQ(expected, actual);
    }
}

static ArrayList* ArrayListTest_generateRandomList(size_t numValues)
{
    ArrayList* retVal = ArrayList_createWithCapacity(numValues);
    for (size_t i = 0; i < numValues; i++)
    {
        int value = rand() % ARRAY_LIST_MAX_RANDOM_VALUE;
        ArrayList_add(retVal, value);
    }
    return retVal;
}

static ArrayList* ArrayListTest_generateZeroList(size_t numValues)
{
    ArrayList* retVal = ArrayList_createWithCapacity(numValues);
    for (size_t i = 0; i < numValues; i++)
    {
        ArrayList_add(retVal, 0);
    }
    return retVal;
}

static void ArrayListTest_verifySorted(ArrayList* pList)
{
    int previous = -1;
    for (size_t i = 0; i < pList->size; i++)
    {
        int curr = ArrayList_get(pList, i);
        ASSERT_TRUE(previous <= curr);
        previous = curr;
    }
}

TEST(ArrayList, TestDefaultConstructor)
{
    ArrayList* list = ArrayList_create();
    ASSERT_EQ(10U, list->capacity);
    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(2U, list->addReallocationMultiplier);
    ASSERT_EQ(4U, list->removeReallocationThreshold);
    ASSERT_EQ(2U, list->removeReallocationDivisor);
    ArrayListTest_smokeTest(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestCustomConstructor)
{
    ArrayList* list = ArrayList_createFull(
        1, 5, 10, 5
    );
    ASSERT_EQ(1U, list->capacity);
    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(5U, list->addReallocationMultiplier);
    ASSERT_EQ(10U, list->removeReallocationThreshold);
    ASSERT_EQ(5U, list->removeReallocationDivisor);
    ArrayListTest_smokeTest(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestCustomConstructorZeroCapacity)
{
    ArrayList* list = ArrayList_createFull(
        0, 2, 7, 4
    );
    ASSERT_EQ(0U, list->capacity);
    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(2U, list->addReallocationMultiplier);
    ASSERT_EQ(7U, list->removeReallocationThreshold);
    ASSERT_EQ(4U, list->removeReallocationDivisor);
    ArrayListTest_smokeTest(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestCustomConstructorRemoveThresholdLowerThanDivisor)
{
    ArrayList* list = ArrayList_createFull(
        10, 2, 4, 100
    );
    ASSERT_EQ(10U, list->capacity);
    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(2U, list->addReallocationMultiplier);
    ASSERT_EQ(200U, list->removeReallocationThreshold);
    ASSERT_EQ(100U, list->removeReallocationDivisor);
    ArrayListTest_smokeTest(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestCreateFromArray)
{
    int values[5] = {0, 1, 2, 3, 4};
    ArrayList* list = ArrayList_createFromArray(
        values, 5
    );
    ASSERT_EQ(5U, list->capacity);
    ASSERT_EQ(5U, list->size);
    ASSERT_EQ(2U, list->addReallocationMultiplier);
    ASSERT_EQ(4U, list->removeReallocationThreshold);
    ASSERT_EQ(2U, list->removeReallocationDivisor);
    ArrayListTest_compareToArray(list, values);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestAddNoReallocation)
{
    ArrayList* list = ArrayList_createFull(
        10, 2, 4, 2
    );

    for (size_t i = 0; i < list->capacity; ++i)
    {
        ArrayList_add(list, i);
        ASSERT_EQ(i+1U, list->size);
        ASSERT_EQ(10U, list->capacity);
    }

    ArrayListTest_checkContents(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestAddOneReallocation)
{
    ArrayList* list = ArrayList_createFull(
        10, 2, 4, 2
    );

    size_t i = 0;
    for (; i < list->capacity; ++i)
    {
        ArrayList_add(list, i);
    }

    ASSERT_EQ(10U, list->size);
    ASSERT_EQ(list->size, list->capacity);
    ArrayListTest_checkContents(list);

    ArrayList_add(list, i);
    ++i;

    ASSERT_NE(list->capacity, list->size);
    ASSERT_EQ(11U, list->size);
    ASSERT_EQ(20U, list->capacity);
    ArrayListTest_checkContents(list);

    for (; i < list->capacity; ++i)
    {
        ArrayList_add(list, i);
    }

    ASSERT_EQ(20U, list->size);
    ASSERT_EQ(list->size, list->capacity);
    ArrayListTest_checkContents(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestAddMultipleReallocation)
{
    ArrayList* list = ArrayList_createFull(
        10, 2, 4, 2
    );

    size_t i = 0;
    for (int iteration = 0; iteration < 10; ++iteration)
    {
        for (; i < list->capacity; ++i)
        {
            ArrayList_add(list, i);
        }

        size_t expectedCapacity = 10 * pow(2, iteration);
        ASSERT_EQ(expectedCapacity, list->capacity);
        ASSERT_EQ(list->capacity, list->size);
        ASSERT_EQ(i, list->size);
        ArrayListTest_checkContents(list);

        ArrayList_add(list, i);
        ++i;

        expectedCapacity = 10 * pow(2, iteration + 1);
        ASSERT_EQ(expectedCapacity, list->capacity);
        ASSERT_NE(list->capacity, list->size);
        ASSERT_EQ(i, list->size);
        ArrayListTest_checkContents(list);
    }
    ArrayList_destroy(list);
}

TEST(ArrayList, TestAddAllTwoEmptyLists)
{
    ArrayList* list1 = ArrayList_create();

    ArrayList* list2 = ArrayList_create();

    ArrayList_addAll(list1, list2);

    ASSERT_EQ(0U, list1->size);
    ArrayListTest_checkContents(list1);
    ASSERT_EQ(0U, list2->size);
    ArrayListTest_checkContents(list2);

    ArrayList_destroy(list1);
    ArrayList_destroy(list2);
}

TEST(ArrayList, TestAddAllFullListToEmptyList)
{
    ArrayList* list1 = ArrayList_create();
    ArrayList* list2 = ArrayList_create();

    for (size_t i = 0; i < 20; ++i)
    {
        ArrayList_add(list2, i);
    }

    ArrayList_addAll(list1, list2);

    ASSERT_EQ(20U, list1->size);
    ArrayListTest_checkContents(list1);
    ASSERT_EQ(20U, list2->size);
    ArrayListTest_checkContents(list2);

    ArrayList_destroy(list1);
    ArrayList_destroy(list2);
}

TEST(ArrayList, TestAddAllEmptyListToFullList)
{
    ArrayList* list1 = ArrayList_create();

    ArrayList* list2 = ArrayList_create();

    for (size_t i = 0; i < 20; ++i)
    {
        ArrayList_add(list1, i);
    }

    ArrayList_addAll(list1, list2);

    ASSERT_EQ(20U, list1->size);
    ArrayListTest_checkContents(list1);
    ASSERT_EQ(0U, list2->size);
    ArrayListTest_checkContents(list2);

    ArrayList_destroy(list1);
    ArrayList_destroy(list2);
}

TEST(ArrayList, TestAddAllFullListToFullList)
{
    ArrayList* list1 = ArrayList_create();
    ArrayList* list2 = ArrayList_create();

    for (size_t i = 0; i < 20; ++i)
    {
        ArrayList_add(list1, i);
        ArrayList_add(list2, i);
    }

    ArrayList_addAll(list1, list2);

    ASSERT_EQ(40U, list1->size);
    for (size_t i = 0; i < 40; ++i)
    {
        size_t expected = i % 20;
        ASSERT_EQ(expected, ArrayList_get(list1, i));
    }

    ASSERT_EQ(20U, list2->size);
    ArrayListTest_checkContents(list2);

    ArrayList_destroy(list1);
    ArrayList_destroy(list2);
}

TEST(ArrayList, TestRemoveNoReallocation)
{
    ArrayList* list = ArrayList_create();

    for (size_t i = 0; i < list->capacity; ++i)
    {
        ArrayList_add(list, i);
    }

    ASSERT_EQ(10U, list->size);
    ASSERT_EQ(list->size, list->capacity);

    size_t oldSize = list->size;
    for (size_t i = 1; i <= oldSize; ++i)
    {
        ArrayList_remove(list, 0);
        ASSERT_EQ(oldSize-i, list->size);
        ASSERT_EQ(oldSize, list->capacity);
        ArrayListTest_checkContents(list, i);
    }

    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(oldSize, list->capacity);

    ArrayList_destroy(list);
}

TEST(ArrayList, TestRemoveNoReallocationFromEndOfList)
{
    ArrayList* list = ArrayList_create();

    for (size_t i = 0; i < list->capacity; ++i)
    {
        ArrayList_add(list, i);
    }

    ASSERT_EQ(10U, list->size);
    ASSERT_EQ(list->size, list->capacity);

    size_t oldSize = list->size;
    for (size_t i = 1; i <= oldSize; ++i)
    {
        ArrayList_remove(list, oldSize - i);
        ASSERT_EQ(oldSize-i, list->size);
        ASSERT_EQ(oldSize, list->capacity);
        ArrayListTest_checkContents(list);
    }

    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(oldSize, list->capacity);

    ArrayList_destroy(list);
}

TEST(ArrayList, TestRemoveOneReallocation)
{
    ArrayList* list = ArrayList_create();

    size_t currCapacity = list->capacity;
    for (size_t i = 0; i < currCapacity + 1; ++i)
    {
        ArrayList_add(list, i);
    }

    size_t oldSize = list->size;
    for (size_t i = 0; i < 6;)
    {
        int valToRemove = ArrayList_get(list, 0);
        ASSERT_EQ(i++, valToRemove);
        ArrayList_remove(list, 0);
        ASSERT_EQ(oldSize-i, list->size);
        ASSERT_EQ(20U, list->capacity);
        ArrayListTest_checkContents(list, i);
    }

    size_t oldCapacity = list->capacity;
    ArrayList_remove(list, 0);

    ASSERT_NE(oldCapacity, list->capacity);
    ASSERT_EQ(10U, list->capacity);
    ASSERT_EQ(4U, list->size);
    ArrayListTest_checkContents(list, 7);

    ArrayList_destroy(list);
}

TEST(ArrayList, TestRemoveMultipleReallocation)
{
    size_t initialCapacity = 10;
    unsigned int removeThreshold = 4;
    unsigned int addRemoveMultiplier = 2;
    size_t maxSize = 160; // 4 reallocations of size 10 with a multiplier of 2

    ArrayList* list = ArrayList_createFull(
        initialCapacity,
        addRemoveMultiplier,
        removeThreshold,
        addRemoveMultiplier
    );

    // Fill the list
    for (size_t i = 0; i < maxSize; ++i)
    {
        ArrayList_add(list, i);
    }

    ArrayListTest_checkContents(list, 0);
    ASSERT_EQ(maxSize, list->size);
    ASSERT_EQ(maxSize, list->capacity);

    // Remove elements up until the next reallocation (160 / 4 = 40)
    size_t i = 0;
    while (list->size > 40)
    {
        ArrayList_remove(list, 0);
        ArrayListTest_checkContents(list, ++i);
        ASSERT_EQ(maxSize - i, list->size);
        ASSERT_EQ(maxSize, list->capacity);
    }

    // The following remove should reduce capacity to 80
    ArrayList_remove(list, 0);
    ArrayListTest_checkContents(list, ++i);
    ASSERT_EQ(maxSize - i, list->size);
    ASSERT_EQ(80U, list->capacity);

    // Remove elements up until the next reallocation (80 / 4 = 20)
    while (list->size > 20)
    {
        ArrayList_remove(list, 0);
        ArrayListTest_checkContents(list, ++i);
        ASSERT_EQ(maxSize - i, list->size);
        ASSERT_EQ(80U, list->capacity);
    }

    // The following remove should reduce capacity to 40
    ArrayList_remove(list, 0);
    ArrayListTest_checkContents(list, ++i);
    ASSERT_EQ(maxSize - i, list->size);
    ASSERT_EQ(40U, list->capacity);

    // Remove elements up until the next reallocation (40 / 4 = 10)
    while (list->size > 10)
    {
        ArrayList_remove(list, 0);
        ArrayListTest_checkContents(list, ++i);
        ASSERT_EQ(maxSize - i, list->size);
        ASSERT_EQ(40U, list->capacity);
    }

    // The following remove should reduce capacity to 20
    ArrayList_remove(list, 0);
    ArrayListTest_checkContents(list, ++i);
    ASSERT_EQ(maxSize - i, list->size);
    ASSERT_EQ(20U, list->capacity);

    // Remove elements up until the next reallocation (20 / 4 = 5)
    while (list->size > 5)
    {
        ArrayList_remove(list, 0);
        ArrayListTest_checkContents(list, ++i);
        ASSERT_EQ(maxSize - i, list->size);
        ASSERT_EQ(20U, list->capacity);
    }

    // The following remove should reduce capacity to 10
    ArrayList_remove(list, 0);
    ArrayListTest_checkContents(list, ++i);
    ASSERT_EQ(maxSize - i, list->size);
    ASSERT_EQ(10U, list->capacity);

    // Remove the remaining elements
    while (list->size > 0)
    {
        ArrayList_remove(list, 0);
        ArrayListTest_checkContents(list, ++i);
        ASSERT_EQ(maxSize - i, list->size);
        ASSERT_EQ(10U, list->capacity);
    }

    // The size should be 0, but the capacity should remain 10
    ArrayList_remove(list, 0);
    ArrayListTest_checkContents(list, ++i);
    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(10U, list->capacity);

    ArrayList_destroy(list);
}

TEST(ArrayList, TestContainsDefaultEmptyList)
{
    ArrayList* list = ArrayList_create();

    int val = 0;
    ASSERT_EQ(FALSE, ArrayList_contains(list, val));

    ArrayList_destroy(list);
}

TEST(ArrayList, TestContainsDefaultSingletonList)
{
    ArrayList* list = ArrayList_create();

    int val = 1;
    ArrayList_add(list, val);
    ASSERT_EQ(TRUE, ArrayList_contains(list, val));

    val = 2;
    ASSERT_EQ(FALSE, ArrayList_contains(list, val));

    ArrayList_destroy(list);
}

TEST(ArrayList, TestContainsDefault)
{
    ArrayList* list = ArrayList_create();

    for (int i = 0; i <= 10; ++i)
    {
        ArrayList_add(list, i);
    }

    for (int i = -5; i < 15; ++i)
    {
        if (i >= 0 && i <= 10) {
            ASSERT_EQ(TRUE, ArrayList_contains(list, i));
        }
        else {
            ASSERT_EQ(FALSE, ArrayList_contains(list, i));
        }
    }

    ArrayList_destroy(list);
}

TEST(ArrayList, TestMethodChaining)
{
    size_t val = 1;
    ArrayList* otherList = ArrayList_create();
    ArrayList_add(otherList, val);
    ArrayList_add(otherList, val);

    /* This just feels dumb...also sad I cannot use literals */
    ArrayList* list = ArrayList_create();
    ArrayList_add(
        ArrayList_add(
            ArrayList_addAll(
                ArrayList_remove(
                    ArrayList_add(
                        ArrayList_add(
                            list,
                            val
                        ),
                        val
                    ),
                    0U
                ),
                otherList
            ),
            val
        ),
        val
    );

    ASSERT_EQ(5U, list->size);

    ArrayList_destroy(list);
    ArrayList_destroy(otherList);
}

TEST(ArrayList, TestAddAllSameList)
{
    size_t val = 1;
    ArrayList* list = ArrayList_create();
    ArrayList_add(list, val);
    ASSERT_EQ(NULL, ArrayList_addAll(list, list));
    ArrayList_destroy(list);
}

TEST(ArrayList, TestMergeSortEmpty)
{
    ArrayList* list = ArrayList_create();
    ArrayList_mergeSort(list);
    ASSERT_EQ(0U, list->size);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestMergeSortSingle)
{
    size_t size = 1U;
    int array[size] = {0};
    ArrayList* list = ArrayList_createFromArray(array, size);

    ArrayList_mergeSort(list);
    ASSERT_EQ(size, list->size);
    ArrayListTest_checkContents(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestMergeSortEven)
{
    size_t size = 4U;
    int array[size] = {3, 1, 2, 0};
    ArrayList* list = ArrayList_createFromArray(array, size);

    ArrayList_mergeSort(list);
    ASSERT_EQ(4U, list->size);
    ArrayListTest_checkContents(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestMergeSortEvenAlreadySorted)
{
    size_t size = 4U;
    int array[size] = {0, 1, 2, 3};
    ArrayList* list = ArrayList_createFromArray(array, size);

    ArrayList_mergeSort(list);
    ASSERT_EQ(4U, list->size);
    ArrayListTest_checkContents(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestMergeSortOdd)
{
    size_t size = 5U;
    int array[size] = {0, 1, 4, 2, 3};
    ArrayList* list = ArrayList_createFromArray(array, size);

    ArrayList_mergeSort(list);
    ASSERT_EQ(5U, list->size);
    ArrayListTest_checkContents(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestMergeSortOddAlreadySorted)
{
    size_t size = 5U;
    int array[size] = {0, 1, 2, 3, 4};
    ArrayList* list = ArrayList_createFromArray(array, size);

    ArrayList_mergeSort(list);
    ASSERT_EQ(size, list->size);
    ArrayListTest_checkContents(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestMergeSortDuplicateValues)
{
    size_t size = 9U;
    int array[size] = {9, 2, 4, 2, 4, 9, 1, 2, 9};
    ArrayList* list = ArrayList_createFromArray(array, size);

    ArrayList_mergeSort(list);
    ASSERT_EQ(size, list->size);

    int expected[size] = {1, 2, 2, 2, 4, 4, 9, 9, 9};
    ArrayListTest_compareToArray(list, expected);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestMergeSortRandomLists)
{
    for (int i = 0; i < NUM_RANDOM_SORT_RUNS; i++)
    {
        ArrayList* list = ArrayListTest_generateRandomList(ARRAY_LIST_RANDOM_SIZE);
        ArrayList_mergeSort(list);
        ArrayListTest_verifySorted(list);

        ArrayList_destroy(list);
    }
}

TEST(ArrayList, TestMergeSortZeroList)
{
    ArrayList* list = ArrayListTest_generateZeroList(ARRAY_LIST_RANDOM_SIZE);
    ArrayList_mergeSort(list);
    ArrayListTest_verifySorted(list);

    ArrayList_destroy(list);
}

TEST(ArrayList, TestQuickSortEmpty)
{
    ArrayList* list = ArrayList_create();
    ArrayList_quickSort(list);
    ASSERT_EQ(0U, list->size);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestQuickSortSingle)
{
    size_t size = 1U;
    int array[size] = {0};
    ArrayList* list = ArrayList_createFromArray(array, size);

    ArrayList_quickSort(list);
    ASSERT_EQ(size, list->size);
    ArrayListTest_checkContents(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestQuickSortEven)
{
    size_t size = 4U;
    int array[size] = {3, 1, 2, 0};
    ArrayList* list = ArrayList_createFromArray(array, size);

    ArrayList_quickSort(list);
    ASSERT_EQ(size, list->size);
    ArrayListTest_checkContents(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestQuickSortEvenAlreadySorted)
{
    size_t size = 4U;
    int array[size] = {0, 1, 2, 3};
    ArrayList* list = ArrayList_createFromArray(array, size);

    ArrayList_quickSort(list);
    ASSERT_EQ(size, list->size);
    ArrayListTest_checkContents(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestQuickSortOdd)
{
    size_t size = 5U;
    int array[size] = {0, 1, 4, 2, 3};
    ArrayList* list = ArrayList_createFromArray(array, size);

    ArrayList_quickSort(list);
    ASSERT_EQ(size, list->size);
    ArrayListTest_checkContents(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestQuickSortOddAlreadySorted)
{
    size_t size = 5U;
    int array[size] = {0, 1, 2, 3, 4};
    ArrayList* list = ArrayList_createFromArray(array, size);

    ArrayList_quickSort(list);
    ASSERT_EQ(size, list->size);
    ArrayListTest_checkContents(list);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestQuickSortDuplicateValues)
{
    size_t size = 9U;
    int array[size] = {9, 2, 4, 2, 4, 9, 1, 2, 9};
    ArrayList* list = ArrayList_createFromArray(array, size);

    ArrayList_quickSort(list);
    ASSERT_EQ(size, list->size);

    int expected[size] = {1, 2, 2, 2, 4, 4, 9, 9, 9};
    ArrayListTest_compareToArray(list, expected);
    ArrayList_destroy(list);
}

TEST(ArrayList, TestQuickSortRandomLists)
{
    for (int i = 0; i < NUM_RANDOM_SORT_RUNS; i++)
    {
        ArrayList* list = ArrayListTest_generateRandomList(ARRAY_LIST_RANDOM_SIZE);
        ArrayList_quickSort(list);
        ArrayListTest_verifySorted(list);

        ArrayList_destroy(list);
    }
}

TEST(ArrayList, TestQuickSortZeroList)
{
    ArrayList* list = ArrayListTest_generateZeroList(ARRAY_LIST_RANDOM_SIZE);
    ArrayList_quickSort(list);
    ArrayListTest_verifySorted(list);

    ArrayList_destroy(list);
}
