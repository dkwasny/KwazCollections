#include "gtest/gtest.h"
#include "ArrayList.h"
#include "Boolean.h"
#include <math.h>
#include <string.h>

static void ArrayListTest_checkContents(ArrayList* list, size_t offset)
{
	for(size_t i = 0; i < list->size; ++i)
	{
		ASSERT_EQ(int(offset + i), ArrayList_get(list, i));
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
	ArrayList* list1 = ArrayList_createFull(
		10, 2, 4, 2
	);

	ArrayList* list2 = ArrayList_createFull(
		10, 2, 4, 2
	);

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
	ArrayList* list1 = ArrayList_createFull(
		10, 2, 4, 2
	);

	ArrayList* list2 = ArrayList_createFull(
		10, 2, 4, 2
	);

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
	ArrayList* list1 = ArrayList_createFull(
		10, 2, 4, 2
	);

	ArrayList* list2 = ArrayList_createFull(
		10, 2, 4, 2
	);

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
	ArrayList* list1 = ArrayList_createFull(
		10, 2, 4, 2
	);

	ArrayList* list2 = ArrayList_createFull(
		10, 2, 4, 2
	);

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
	ArrayList* list = ArrayList_createFull(
		10, 2, 4, 2
	);

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
	ArrayList* list = ArrayList_createFull(
		10, 2, 4, 2
	);

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
	ArrayList* list = ArrayList_createFull(
		10, 2, 4, 2
	);

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
	ArrayList* list = ArrayList_create();

	size_t value = 0U;
	ArrayList_add(list, value);
	
	ArrayList_mergeSort(list);
	ASSERT_EQ(1U, list->size);
	ArrayListTest_checkContents(list);
	ArrayList_destroy(list);
}

TEST(ArrayList, TestMergeSortEven)
{
	ArrayList* list = ArrayList_create();

	size_t value = 3U;
	ArrayList_add(list, value);
	value = 1U;
	ArrayList_add(list, value);
	value = 2U;
	ArrayList_add(list, value);
	value = 0U;
	ArrayList_add(list, value);
	
	ArrayList_mergeSort(list);
	ASSERT_EQ(4U, list->size);
	ArrayListTest_checkContents(list);
	ArrayList_destroy(list);
}

TEST(ArrayList, TestMergeSortEvenAlreadySorted)
{
	ArrayList* list = ArrayList_create();

	size_t value = 0U;
	ArrayList_add(list, value);
	value = 1U;
	ArrayList_add(list, value);
	value = 2U;
	ArrayList_add(list, value);
	value = 3U;
	ArrayList_add(list, value);
	
	ArrayList_mergeSort(list);
	ASSERT_EQ(4U, list->size);
	ArrayListTest_checkContents(list);
	ArrayList_destroy(list);
}

TEST(ArrayList, TestMergeSortOdd)
{
	ArrayList* list = ArrayList_create();

	size_t value = 0U;
	ArrayList_add(list, value);
	value = 1U;
	ArrayList_add(list, value);
	value = 4U;
	ArrayList_add(list, value);
	value = 2U;
	ArrayList_add(list, value);
	value = 3U;
	ArrayList_add(list, value);
	
	ArrayList_mergeSort(list);
	ASSERT_EQ(5U, list->size);
	ArrayListTest_checkContents(list);
	ArrayList_destroy(list);
}

TEST(ArrayList, TestMergeSortOddAlreadySorted)
{
	ArrayList* list = ArrayList_create();

	size_t value = 0U;
	ArrayList_add(list, value);
	value = 1U;
	ArrayList_add(list, value);
	value = 2U;
	ArrayList_add(list, value);
	value = 3U;
	ArrayList_add(list, value);
	value = 4U;
	ArrayList_add(list, value);
	
	ArrayList_mergeSort(list);
	ASSERT_EQ(5U, list->size);
	ArrayListTest_checkContents(list);
	ArrayList_destroy(list);
}
