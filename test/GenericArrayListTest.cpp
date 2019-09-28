#include "gtest/gtest.h"
#include "GenericArrayList.h"
#include "Boolean.h"
#include <math.h>
#include <string.h>

static void GenericArrayListTest_checkContents(GenericArrayList* list, size_t offset)
{
	for(size_t i = 0; i < list->size; ++i)
	{
		ASSERT_EQ(offset + i, *((size_t*)GenericArrayList_get(list, i)));
	}	
}

static void GenericArrayListTest_checkContents(GenericArrayList* list)
{
	GenericArrayListTest_checkContents(list, 0);
}

static void GenericArrayListTest_smokeTest(GenericArrayList* list)
{
	for (size_t i = 0; i < 20; ++i)
	{
		GenericArrayList_add(list, &i);
	}
	GenericArrayListTest_checkContents(list);
	for (size_t i = 0; i < 20; ++i)
	{
		GenericArrayList_remove(list, 0);
	}
}

static void GenericArrayListTest_smokeTestIterator(GenericIterator* iter, size_t expectedElements)
{
	size_t i = 0;
	while (iter->hasNext(iter))
	{
		ASSERT_EQ(i, *((size_t*)iter->peekNext(iter)));
		ASSERT_EQ(i, *((size_t*)iter->next(iter)));
		++i;
	}

	ASSERT_EQ(expectedElements, i);
}

TEST(GenericArrayList, TestDefaultConstructor)
{
	GenericArrayList* list = GenericArrayList_createDefault(sizeof(size_t));
	ASSERT_EQ(10U, list->capacity);
	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(2U, list->addReallocationMultiplier);
	ASSERT_EQ(4U, list->removeReallocationThreshold);
	ASSERT_EQ(2U, list->removeReallocationDivisor);
	GenericArrayListTest_smokeTest(list);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestCustomConstructor)
{
	GenericArrayList* list = GenericArrayList_createFull(
		sizeof(size_t),
		1, 5, 10, 5,
		memcmp,
		memcpy
	);
	ASSERT_EQ(1U, list->capacity);
	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(5U, list->addReallocationMultiplier);
	ASSERT_EQ(10U, list->removeReallocationThreshold);
	ASSERT_EQ(5U, list->removeReallocationDivisor);
	GenericArrayListTest_smokeTest(list);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestCustomConstructorZeroCapacity)
{
	GenericArrayList* list = GenericArrayList_createFull(
		sizeof(size_t),
		0, 2, 7, 4,
		memcmp,
		memcpy
	);
	ASSERT_EQ(0U, list->capacity);
	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(2U, list->addReallocationMultiplier);
	ASSERT_EQ(7U, list->removeReallocationThreshold);
	ASSERT_EQ(4U, list->removeReallocationDivisor);
	GenericArrayListTest_smokeTest(list);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestCustomConstructorRemoveThresholdLowerThanDivisor)
{
	GenericArrayList* list = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 100,
		memcmp,
		memcpy
	);
	ASSERT_EQ(10U, list->capacity);
	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(2U, list->addReallocationMultiplier);
	ASSERT_EQ(200U, list->removeReallocationThreshold);
	ASSERT_EQ(100U, list->removeReallocationDivisor);
	GenericArrayListTest_smokeTest(list);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestAddNoReallocation)
{
	GenericArrayList* list = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	for (size_t i = 0; i < list->capacity; ++i)
	{
		GenericArrayList_add(list, &i);
		ASSERT_EQ(i+1U, list->size);
		ASSERT_EQ(10U, list->capacity);
	}

	GenericArrayListTest_checkContents(list);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestAddOneReallocation)
{
	GenericArrayList* list = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	size_t i = 0;
	for (; i < list->capacity; ++i)
	{
		GenericArrayList_add(list, &i);
	}

	ASSERT_EQ(10U, list->size);
	ASSERT_EQ(list->size, list->capacity);
	GenericArrayListTest_checkContents(list);
	
	GenericArrayList_add(list, &i);
	++i;

	ASSERT_NE(list->capacity, list->size);
	ASSERT_EQ(11U, list->size);
	ASSERT_EQ(20U, list->capacity);
	GenericArrayListTest_checkContents(list);

	for (; i < list->capacity; ++i)
	{
		GenericArrayList_add(list, &i);
	}

	ASSERT_EQ(20U, list->size);
	ASSERT_EQ(list->size, list->capacity);
	GenericArrayListTest_checkContents(list);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestAddMultipleReallocation)
{
	GenericArrayList* list = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);
	
	size_t i = 0;
	for (int iteration = 0; iteration < 10; ++iteration)
	{
		for (; i < list->capacity; ++i)
		{
			GenericArrayList_add(list, &i);
		}

		size_t expectedCapacity = 10 * pow(2, iteration);
		ASSERT_EQ(expectedCapacity, list->capacity);
		ASSERT_EQ(list->capacity, list->size);	
		ASSERT_EQ(i, list->size);
		GenericArrayListTest_checkContents(list);

		GenericArrayList_add(list, &i);
		++i;

		expectedCapacity = 10 * pow(2, iteration + 1);
		ASSERT_EQ(expectedCapacity, list->capacity);
		ASSERT_NE(list->capacity, list->size);	
		ASSERT_EQ(i, list->size);
		GenericArrayListTest_checkContents(list);
	}
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestAddAllTwoEmptyLists)
{
	GenericArrayList* list1 = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	GenericArrayList* list2 = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	GenericArrayList_addAll(list1, list2);

	ASSERT_EQ(0U, list1->size);
	GenericArrayListTest_checkContents(list1);
	ASSERT_EQ(0U, list2->size);
	GenericArrayListTest_checkContents(list2);

	GenericArrayList_destroy(list1);
	GenericArrayList_destroy(list2);
}

TEST(GenericArrayList, TestAddAllFullListToEmptyList)
{
	GenericArrayList* list1 = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	GenericArrayList* list2 = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	for (size_t i = 0; i < 20; ++i)
	{
		GenericArrayList_add(list2, &i);
	}

	GenericArrayList_addAll(list1, list2);

	ASSERT_EQ(20U, list1->size);
	GenericArrayListTest_checkContents(list1);
	ASSERT_EQ(20U, list2->size);
	GenericArrayListTest_checkContents(list2);
	
	GenericArrayList_destroy(list1);
	GenericArrayList_destroy(list2);
}

TEST(GenericArrayList, TestAddAllEmptyListToFullList)
{
	GenericArrayList* list1 = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	GenericArrayList* list2 = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	for (size_t i = 0; i < 20; ++i)
	{
		GenericArrayList_add(list1, &i);
	}

	GenericArrayList_addAll(list1, list2);

	ASSERT_EQ(20U, list1->size);
	GenericArrayListTest_checkContents(list1);
	ASSERT_EQ(0U, list2->size);
	GenericArrayListTest_checkContents(list2);
	
	GenericArrayList_destroy(list1);
	GenericArrayList_destroy(list2);
}

TEST(GenericArrayList, TestAddAllFullListToFullList)
{
	GenericArrayList* list1 = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	GenericArrayList* list2 = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	for (size_t i = 0; i < 20; ++i)
	{
		GenericArrayList_add(list1, &i);
		GenericArrayList_add(list2, &i);
	}

	GenericArrayList_addAll(list1, list2);

	ASSERT_EQ(40U, list1->size);
	for (size_t i = 0; i < 40; ++i)
	{
		size_t expected = i % 20;
		ASSERT_EQ(expected, *((size_t*)(GenericArrayList_get(list1, i))));
	}	

	ASSERT_EQ(20U, list2->size);
	GenericArrayListTest_checkContents(list2);
	
	GenericArrayList_destroy(list1);
	GenericArrayList_destroy(list2);
}

TEST(GenericArrayList, TestAddIterator)
{
	GenericArrayList* list1 = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);
	for (size_t i = 0; i < 10; ++i)
	{
		GenericArrayList_add(list1, &i);
	}

	GenericArrayList* list2 = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);
	for (size_t i = 10; i < 20; ++i)
	{
		GenericArrayList_add(list2, &i);
	}

	GenericIterator* iter2 = GenericArrayList_iterator(list2);	
	GenericArrayList_addIterator(list1, iter2);
	iter2->destroy(iter2);
	ASSERT_EQ(20U, list1->size);
	
	GenericIterator* iter = GenericArrayList_iterator(list1);
	for (size_t i = 0; iter->hasNext(iter); ++i)
	{
		ASSERT_EQ(i, *((size_t*)iter->next(iter)));
	}
	iter->destroy(iter);
	
	GenericArrayList_destroy(list1);
	GenericArrayList_destroy(list2);
}

TEST(GenericArrayList, TestRemoveNoReallocation)
{
	GenericArrayList* list = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	for (size_t i = 0; i < list->capacity; ++i)
	{
		GenericArrayList_add(list, &i);
	}

	ASSERT_EQ(10U, list->size);
	ASSERT_EQ(list->size, list->capacity);

	size_t oldSize = list->size;
	for (size_t i = 1; i <= oldSize; ++i)
	{
		GenericArrayList_remove(list, 0);
		ASSERT_EQ(oldSize-i, list->size);
		ASSERT_EQ(oldSize, list->capacity);
		GenericArrayListTest_checkContents(list, i);
	}

	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(oldSize, list->capacity);

	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestRemoveNoReallocationFromEndOfList)
{
	GenericArrayList* list = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	for (size_t i = 0; i < list->capacity; ++i)
	{
		GenericArrayList_add(list, &i);
	}

	ASSERT_EQ(10U, list->size);
	ASSERT_EQ(list->size, list->capacity);

	size_t oldSize = list->size;
	for (size_t i = 1; i <= oldSize; ++i)
	{
		GenericArrayList_remove(list, oldSize - i);
		ASSERT_EQ(oldSize-i, list->size);
		ASSERT_EQ(oldSize, list->capacity);
		GenericArrayListTest_checkContents(list);
	}

	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(oldSize, list->capacity);

	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestRemoveOneReallocation)
{
	GenericArrayList* list = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	size_t currCapacity = list->capacity;
	for (size_t i = 0; i < currCapacity + 1; ++i)
	{
		GenericArrayList_add(list, &i);
	}

	size_t oldSize = list->size;
	for (size_t i = 0; i < 6;)
	{
		size_t* valToRemove = (size_t*)GenericArrayList_get(list, 0);
		ASSERT_EQ(i++, *valToRemove);
		GenericArrayList_remove(list, 0);
		ASSERT_EQ(oldSize-i, list->size);
		ASSERT_EQ(20U, list->capacity);
		GenericArrayListTest_checkContents(list, i);
	}
	
	size_t oldCapacity = list->capacity;
	GenericArrayList_remove(list, 0);

	ASSERT_NE(oldCapacity, list->capacity);
	ASSERT_EQ(10U, list->capacity);
	ASSERT_EQ(4U, list->size);
	GenericArrayListTest_checkContents(list, 7);

	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestRemoveMultipleReallocation)
{
	size_t initialCapacity = 10;
	unsigned int removeThreshold = 4;
	unsigned int addRemoveMultiplier = 2;
	size_t maxSize = 160; // 4 reallocations of size 10 with a multiplier of 2
	
	GenericArrayList* list = GenericArrayList_createFull(
		sizeof(size_t),
		initialCapacity,
		addRemoveMultiplier,
		removeThreshold,
		addRemoveMultiplier,
		memcmp,
		memcpy
	);

	// Fill the list
	for (size_t i = 0; i < maxSize; ++i)
	{
		GenericArrayList_add(list, &i);
	}

	GenericArrayListTest_checkContents(list, 0);
	ASSERT_EQ(maxSize, list->size);
	ASSERT_EQ(maxSize, list->capacity);

	// Remove elements up until the next reallocation (160 / 4 = 40)
	size_t i = 0;
	while (list->size > 40)
	{
		GenericArrayList_remove(list, 0);
		GenericArrayListTest_checkContents(list, ++i);
		ASSERT_EQ(maxSize - i, list->size);
		ASSERT_EQ(maxSize, list->capacity);
	}

	// The following remove should reduce capacity to 80
	GenericArrayList_remove(list, 0);
	GenericArrayListTest_checkContents(list, ++i);
	ASSERT_EQ(maxSize - i, list->size);
	ASSERT_EQ(80U, list->capacity);

	// Remove elements up until the next reallocation (80 / 4 = 20)
	while (list->size > 20)
	{
		GenericArrayList_remove(list, 0);
		GenericArrayListTest_checkContents(list, ++i);
		ASSERT_EQ(maxSize - i, list->size);
		ASSERT_EQ(80U, list->capacity);
	}
	
	// The following remove should reduce capacity to 40
	GenericArrayList_remove(list, 0);
	GenericArrayListTest_checkContents(list, ++i);
	ASSERT_EQ(maxSize - i, list->size);
	ASSERT_EQ(40U, list->capacity);
	
	// Remove elements up until the next reallocation (40 / 4 = 10)
	while (list->size > 10)
	{
		GenericArrayList_remove(list, 0);
		GenericArrayListTest_checkContents(list, ++i);
		ASSERT_EQ(maxSize - i, list->size);
		ASSERT_EQ(40U, list->capacity);
	}
	
	// The following remove should reduce capacity to 20
	GenericArrayList_remove(list, 0);
	GenericArrayListTest_checkContents(list, ++i);
	ASSERT_EQ(maxSize - i, list->size);
	ASSERT_EQ(20U, list->capacity);
	
	// Remove elements up until the next reallocation (20 / 4 = 5)
	while (list->size > 5)
	{
		GenericArrayList_remove(list, 0);
		GenericArrayListTest_checkContents(list, ++i);
		ASSERT_EQ(maxSize - i, list->size);
		ASSERT_EQ(20U, list->capacity);
	}
	
	// The following remove should reduce capacity to 10
	GenericArrayList_remove(list, 0);
	GenericArrayListTest_checkContents(list, ++i);
	ASSERT_EQ(maxSize - i, list->size);
	ASSERT_EQ(10U, list->capacity);
	
	// Remove the remaining elements
	while (list->size > 0)
	{
		GenericArrayList_remove(list, 0);
		GenericArrayListTest_checkContents(list, ++i);
		ASSERT_EQ(maxSize - i, list->size);
		ASSERT_EQ(10U, list->capacity);
	}
	
	// The size should be 0, but the capacity should remain 10
	GenericArrayList_remove(list, 0);
	GenericArrayListTest_checkContents(list, ++i);
	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(10U, list->capacity);

	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestIteratorNextOperations)
{
	GenericArrayList* list = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	const size_t expectedSize = 50;	
	for (size_t i = 0; i < expectedSize; ++i)
	{
		GenericArrayList_add(list, &i);
	}

	GenericIterator* iter = GenericArrayList_iterator(list);
	ASSERT_TRUE(iter->hasNext(iter));
	ASSERT_EQ(0U, *(size_t*)iter->peekNext(iter));

	size_t times = 0;
	while(iter->hasNext(iter))
	{
		ASSERT_EQ(times, *((size_t*)iter->peekNext(iter)));
		ASSERT_EQ(times, *((size_t*)iter->next(iter)));
		++times;
	}

	ASSERT_FALSE(iter->hasNext(iter));
	ASSERT_EQ(NULL, iter->peekNext(iter));
	ASSERT_EQ(NULL, iter->next(iter));
	ASSERT_EQ(expectedSize, times);

	iter->destroy(iter);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestIteratorEmptyList)
{
	GenericArrayList* list = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);
	GenericIterator* iter = GenericArrayList_iterator(list);
	
	ASSERT_FALSE(iter->hasNext(iter));
	ASSERT_EQ(NULL, iter->peekNext(iter));
	ASSERT_EQ(NULL, iter->next(iter));
	
	/*
 	* Many calls to remove on an "empty" iterator
 	* should have no effect on the list.
 	*/
	iter->remove(iter);
	iter->remove(iter);
	iter->remove(iter);

	ASSERT_FALSE(iter->hasNext(iter));
	ASSERT_EQ(NULL, iter->peekNext(iter));
	ASSERT_EQ(NULL, iter->next(iter));
	
	iter->destroy(iter);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestIteratorCreatedBeforeListModification)
{
	GenericArrayList* list = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);
	GenericIterator* iter = GenericArrayList_iterator(list);

	const size_t expectedSize = 50;	
	for (size_t i = 0; i < expectedSize; ++i)
	{
		GenericArrayList_add(list, &i);
	}

	GenericArrayListTest_smokeTestIterator(iter, expectedSize);

	iter->destroy(iter);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestIteratorRemoveFirstElementRepeated)
{
	GenericArrayList* list = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	const size_t listSize = 50;	
	for (size_t i = 0; i < listSize; ++i)
	{
		GenericArrayList_add(list, &i);
	}

	GenericIterator* iter = GenericArrayList_iterator(list);
	for (size_t i = 0; i < listSize; ++i)
	{
		ASSERT_EQ(listSize - i, list->size);
		GenericArrayListTest_checkContents(list, i);

		iter->next(iter);
		iter->remove(iter);

		ASSERT_EQ(listSize - i - 1, list->size);
		GenericArrayListTest_checkContents(list, i + 1);
	}

	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(10U, list->capacity);

	iter->destroy(iter);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestIteratorRemoveLastElementRepeated)
{
	GenericArrayList* list = GenericArrayList_createFull(
		sizeof(size_t),
		10, 2, 4, 2,
		memcmp,
		memcpy
	);

	const size_t listSize = 50;	
	for (size_t i = 0; i < listSize; ++i)
	{
		GenericArrayList_add(list, &i);
	}

	GenericIterator* iter = GenericArrayList_iterator(list);
	while (iter->hasNext(iter))
	{
		iter->next(iter);
	}

	for (size_t i = 0; i < listSize; ++i)
	{
		ASSERT_FALSE(iter->hasNext(iter));
		
		ASSERT_EQ(listSize - i, list->size);
		GenericArrayListTest_checkContents(list);

		iter->remove(iter);

		ASSERT_EQ(listSize - i - 1, list->size);
		GenericArrayListTest_checkContents(list);
	}

	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(10U, list->capacity);

	iter->destroy(iter);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestContainsDefaultEmptyList)
{
	GenericArrayList* list = GenericArrayList_createDefault(sizeof(int));

	int val = 0;
	ASSERT_EQ(FALSE, GenericArrayList_contains(list, &val));

	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestContainsDefaultSingletonList)
{
	GenericArrayList* list = GenericArrayList_createDefault(sizeof(int));

	int val = 1;
	GenericArrayList_add(list, &val);
	ASSERT_EQ(TRUE, GenericArrayList_contains(list, &val));

	val = 2;
	ASSERT_EQ(FALSE, GenericArrayList_contains(list, &val));

	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestContainsDefault)
{
	GenericArrayList* list = GenericArrayList_createDefault(sizeof(int));

	for (int i = 0; i <= 10; ++i)
	{
		GenericArrayList_add(list, &i);
	}

	for (int i = -5; i < 15; ++i)
	{
		if (i >= 0 && i <= 10) {
			ASSERT_EQ(TRUE, GenericArrayList_contains(list, &i));
		}
		else {
			ASSERT_EQ(FALSE, GenericArrayList_contains(list, &i));
		}
	}

	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestMethodChaining)
{
	size_t val = 1;
	GenericArrayList* otherList = GenericArrayList_createDefault(sizeof(size_t));
	GenericArrayList_add(otherList, &val);
	GenericArrayList_add(otherList, &val);

	/* This just feels dumb...also sad I cannot use literals */
	GenericArrayList* list = GenericArrayList_createDefault(sizeof(size_t));
	GenericArrayList_add(
		GenericArrayList_add(
			GenericArrayList_addAll(
				GenericArrayList_remove(
					GenericArrayList_add(
						GenericArrayList_add(
							list,
							&val
						),
						&val
					),
					0U
				),
				otherList
			),
			&val
		),
		&val
	);
	
	ASSERT_EQ(5U, list->size);
	
	GenericArrayList_destroy(list);
	GenericArrayList_destroy(otherList);
}

TEST(GenericArrayList, TestAddAllSameList)
{
	size_t val = 1;
	GenericArrayList* list = GenericArrayList_createDefault(sizeof(size_t));
	GenericArrayList_add(list, &val);
	ASSERT_EQ(NULL, GenericArrayList_addAll(list, list));
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestCopyList)
{
	GenericArrayList* list = GenericArrayList_createDefault(sizeof(size_t));
	for (size_t i = 0; i < 100; ++i)
	{
		GenericArrayList_add(list, &i);
	}

	GenericIterator* copyIter = GenericArrayList_iterator(list);	
	GenericArrayList* otherList = GenericArrayList_addIterator(
		GenericArrayList_createDefault(sizeof(size_t)),
		copyIter
	);
	copyIter->destroy(copyIter);
	
	ASSERT_EQ(list->size, otherList->size);
	
	GenericIterator* iter = GenericArrayList_iterator(list);
	GenericIterator* otherIter = GenericArrayList_iterator(otherList);
	
	while (iter->hasNext(iter))
	{
		ASSERT_EQ(
			*(size_t*)iter->next(iter),
			*(size_t*)otherIter->next(otherIter)
		);
	}
	
	ASSERT_FALSE(otherIter->hasNext(otherIter));
	
	iter->destroy(iter);
	otherIter->destroy(otherIter);
	GenericArrayList_destroy(list);
	GenericArrayList_destroy(otherList);
}

TEST(GenericArrayList, TestMergeSortEmpty)
{
	GenericArrayList* list = GenericArrayList_createDefault(sizeof(size_t));
	GenericArrayList_mergeSort(list);
	ASSERT_EQ(0U, list->size);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestMergeSortSingle)
{
	GenericArrayList* list = GenericArrayList_createDefault(sizeof(size_t));

	size_t value = 0U;
	GenericArrayList_add(list, &value);
	
	GenericArrayList_mergeSort(list);
	ASSERT_EQ(1U, list->size);
	GenericArrayListTest_checkContents(list);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestMergeSortEven)
{
	GenericArrayList* list = GenericArrayList_createDefault(sizeof(size_t));

	size_t value = 3U;
	GenericArrayList_add(list, &value);
	value = 1U;
	GenericArrayList_add(list, &value);
	value = 2U;
	GenericArrayList_add(list, &value);
	value = 0U;
	GenericArrayList_add(list, &value);
	
	GenericArrayList_mergeSort(list);
	ASSERT_EQ(4U, list->size);
	GenericArrayListTest_checkContents(list);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestMergeSortEvenAlreadySorted)
{
	GenericArrayList* list = GenericArrayList_createDefault(sizeof(size_t));

	size_t value = 0U;
	GenericArrayList_add(list, &value);
	value = 1U;
	GenericArrayList_add(list, &value);
	value = 2U;
	GenericArrayList_add(list, &value);
	value = 3U;
	GenericArrayList_add(list, &value);
	
	GenericArrayList_mergeSort(list);
	ASSERT_EQ(4U, list->size);
	GenericArrayListTest_checkContents(list);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestMergeSortOdd)
{
	GenericArrayList* list = GenericArrayList_createDefault(sizeof(size_t));

	size_t value = 0U;
	GenericArrayList_add(list, &value);
	value = 1U;
	GenericArrayList_add(list, &value);
	value = 4U;
	GenericArrayList_add(list, &value);
	value = 2U;
	GenericArrayList_add(list, &value);
	value = 3U;
	GenericArrayList_add(list, &value);
	
	GenericArrayList_mergeSort(list);
	ASSERT_EQ(5U, list->size);
	GenericArrayListTest_checkContents(list);
	GenericArrayList_destroy(list);
}

TEST(GenericArrayList, TestMergeSortOddAlreadySorted)
{
	GenericArrayList* list = GenericArrayList_createDefault(sizeof(size_t));

	size_t value = 0U;
	GenericArrayList_add(list, &value);
	value = 1U;
	GenericArrayList_add(list, &value);
	value = 2U;
	GenericArrayList_add(list, &value);
	value = 3U;
	GenericArrayList_add(list, &value);
	value = 4U;
	GenericArrayList_add(list, &value);
	
	GenericArrayList_mergeSort(list);
	ASSERT_EQ(5U, list->size);
	GenericArrayListTest_checkContents(list);
	GenericArrayList_destroy(list);
}
