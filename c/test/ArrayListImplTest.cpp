#include "gtest/gtest.h"
#include "ArrayListImpl.h"

static void ArrayListImplTest_checkContents(ArrayListImpl* list, size_t offset)
{
	for(size_t i = 0; i < list->size; ++i)
	{
		ASSERT_EQ(offset + i, *((size_t*)ArrayListImpl_get(list, i)));
	}	
}

static void ArrayListImplTest_checkContents(ArrayListImpl* list)
{
	ArrayListImplTest_checkContents(list, 0);
}

static void ArrayListImplTest_smokeTest(ArrayListImpl* list)
{
	for (size_t i = 0; i < 20; ++i)
	{
		ArrayListImpl_add(list, &i);
	}
	ArrayListImplTest_checkContents(list);
	for (size_t i = 0; i < 20; ++i)
	{
		ArrayListImpl_remove(list, 0);
	}
}

static void ArrayListTest_smokeTestIterator(ArrayListImplIterator* iter, size_t expectedElements)
{
	size_t i = 0;
	while (ArrayListImplIterator_hasNext(iter))
	{
		ASSERT_EQ(i, *((size_t*)ArrayListImplIterator_peekNext(iter)));
		ASSERT_EQ(i, *((size_t*)ArrayListImplIterator_next(iter)));
		++i;
	}

	ASSERT_EQ(expectedElements, i);
}

TEST(ArrayListImpl, TestDefaultConstructor)
{
	ArrayListImpl* list = ArrayListImpl_createDefault(sizeof(size_t));
	ASSERT_EQ(10U, list->capacity);
	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(2U, list->addReallocationMultiplier);
	ASSERT_EQ(4U, list->removeReallocationThreshold);
	ASSERT_EQ(2U, list->removeReallocationDivisor);
	ArrayListImplTest_smokeTest(list);
	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestCustomConstructor)
{
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		1, 5, 10, 5
	);
	ASSERT_EQ(1U, list->capacity);
	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(5U, list->addReallocationMultiplier);
	ASSERT_EQ(10U, list->removeReallocationThreshold);
	ASSERT_EQ(5U, list->removeReallocationDivisor);
	ArrayListImplTest_smokeTest(list);
	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestCustomConstructorZeroCapacity)
{
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		0, 2, 7, 4
	);
	ASSERT_EQ(0U, list->capacity);
	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(2U, list->addReallocationMultiplier);
	ASSERT_EQ(7U, list->removeReallocationThreshold);
	ASSERT_EQ(4U, list->removeReallocationDivisor);
	ArrayListImplTest_smokeTest(list);
	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestCustomConstructorRemoveThresholdLowerThanDivisor)
{
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 100
	);
	ASSERT_EQ(10U, list->capacity);
	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(2U, list->addReallocationMultiplier);
	ASSERT_EQ(200U, list->removeReallocationThreshold);
	ASSERT_EQ(100U, list->removeReallocationDivisor);
	ArrayListImplTest_smokeTest(list);
	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestAddNoReallocation)
{
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	for (size_t i = 0; i < list->capacity; ++i)
	{
		ArrayListImpl_add(list, &i);
		ASSERT_EQ(i+1U, list->size);
		ASSERT_EQ(10U, list->capacity);
	}

	ArrayListImplTest_checkContents(list);
	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestAddOneReallocation)
{
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	size_t i = 0;
	for (; i < list->capacity; ++i)
	{
		ArrayListImpl_add(list, &i);
	}

	ASSERT_EQ(10U, list->size);
	ASSERT_EQ(list->size, list->capacity);
	ArrayListImplTest_checkContents(list);
	
	ArrayListImpl_add(list, &i);
	++i;

	ASSERT_NE(list->capacity, list->size);
	ASSERT_EQ(11U, list->size);
	ASSERT_EQ(20U, list->capacity);
	ArrayListImplTest_checkContents(list);

	for (; i < list->capacity; ++i)
	{
		ArrayListImpl_add(list, &i);
	}

	ASSERT_EQ(20U, list->size);
	ASSERT_EQ(list->size, list->capacity);
	ArrayListImplTest_checkContents(list);
	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestAddMultipleReallocation)
{
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);
	
	size_t i = 0;
	for (int iteration = 0; iteration < 10; ++iteration)
	{
		for (; i < list->capacity; ++i)
		{
			ArrayListImpl_add(list, &i);
		}

		size_t expectedCapacity = 10 * pow(2, iteration);
		ASSERT_EQ(expectedCapacity, list->capacity);
		ASSERT_EQ(list->capacity, list->size);	
		ASSERT_EQ(i, list->size);
		ArrayListImplTest_checkContents(list);

		ArrayListImpl_add(list, &i);
		++i;

		expectedCapacity = 10 * pow(2, iteration + 1);
		ASSERT_EQ(expectedCapacity, list->capacity);
		ASSERT_NE(list->capacity, list->size);	
		ASSERT_EQ(i, list->size);
		ArrayListImplTest_checkContents(list);
	}
	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestAddAllTwoEmptyLists)
{
	ArrayListImpl* list1 = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	ArrayListImpl* list2 = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	ArrayListImpl_addAll(list1, list2);

	ASSERT_EQ(0U, list1->size);
	ArrayListImplTest_checkContents(list1);
	ASSERT_EQ(0U, list2->size);
	ArrayListImplTest_checkContents(list2);

	ArrayListImpl_destroy(list1);
	ArrayListImpl_destroy(list2);
}

TEST(ArrayListImpl, TestAddAllFullListToEmptyList)
{
	ArrayListImpl* list1 = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	ArrayListImpl* list2 = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	for (size_t i = 0; i < 20; ++i)
	{
		ArrayListImpl_add(list2, &i);
	}

	ArrayListImpl_addAll(list1, list2);

	ASSERT_EQ(20U, list1->size);
	ArrayListImplTest_checkContents(list1);
	ASSERT_EQ(20U, list2->size);
	ArrayListImplTest_checkContents(list2);
	
	ArrayListImpl_destroy(list1);
	ArrayListImpl_destroy(list2);
}

TEST(ArrayListImpl, TestAddAllEmptyListToFullList)
{
	ArrayListImpl* list1 = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	ArrayListImpl* list2 = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	for (size_t i = 0; i < 20; ++i)
	{
		ArrayListImpl_add(list1, &i);
	}

	ArrayListImpl_addAll(list1, list2);

	ASSERT_EQ(20U, list1->size);
	ArrayListImplTest_checkContents(list1);
	ASSERT_EQ(0U, list2->size);
	ArrayListImplTest_checkContents(list2);
	
	ArrayListImpl_destroy(list1);
	ArrayListImpl_destroy(list2);
}

TEST(ArrayListImpl, TestAddAllFullListToFullList)
{
	ArrayListImpl* list1 = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	ArrayListImpl* list2 = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	for (size_t i = 0; i < 20; ++i)
	{
		ArrayListImpl_add(list1, &i);
		ArrayListImpl_add(list2, &i);
	}

	ArrayListImpl_addAll(list1, list2);

	ASSERT_EQ(40U, list1->size);
	for (size_t i = 0; i < 40; ++i)
	{
		size_t expected = i % 20;
		ASSERT_EQ(expected, *((size_t*)(ArrayListImpl_get(list1, i))));
	}	

	ASSERT_EQ(20U, list2->size);
	ArrayListImplTest_checkContents(list2);
	
	ArrayListImpl_destroy(list1);
	ArrayListImpl_destroy(list2);
}

TEST(ArrayListImpl, TestRemoveNoReallocation)
{
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	for (size_t i = 0; i < list->capacity; ++i)
	{
		ArrayListImpl_add(list, &i);
	}

	ASSERT_EQ(10U, list->size);
	ASSERT_EQ(list->size, list->capacity);

	size_t oldSize = list->size;
	for (size_t i = 1; i <= oldSize; ++i)
	{
		ArrayListImpl_remove(list, 0);
		ASSERT_EQ(oldSize-i, list->size);
		ASSERT_EQ(oldSize, list->capacity);
		ArrayListImplTest_checkContents(list, i);
	}

	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(oldSize, list->capacity);

	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestRemoveNoReallocationFromEndOfList)
{
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	for (size_t i = 0; i < list->capacity; ++i)
	{
		ArrayListImpl_add(list, &i);
	}

	ASSERT_EQ(10U, list->size);
	ASSERT_EQ(list->size, list->capacity);

	size_t oldSize = list->size;
	for (size_t i = 1; i <= oldSize; ++i)
	{
		ArrayListImpl_remove(list, oldSize - i);
		ASSERT_EQ(oldSize-i, list->size);
		ASSERT_EQ(oldSize, list->capacity);
		ArrayListImplTest_checkContents(list);
	}

	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(oldSize, list->capacity);

	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestRemoveOneReallocation)
{
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	size_t currCapacity = list->capacity;
	for (size_t i = 0; i < currCapacity + 1; ++i)
	{
		ArrayListImpl_add(list, &i);
	}

	size_t oldSize = list->size;
	for (size_t i = 0; i < 6;)
	{
		size_t* valToRemove = (size_t*)ArrayListImpl_get(list, 0);
		ASSERT_EQ(i++, *valToRemove);
		ArrayListImpl_remove(list, 0);
		ASSERT_EQ(oldSize-i, list->size);
		ASSERT_EQ(20U, list->capacity);
		ArrayListImplTest_checkContents(list, i);
	}
	
	size_t oldCapacity = list->capacity;
	ArrayListImpl_remove(list, 0);

	ASSERT_NE(oldCapacity, list->capacity);
	ASSERT_EQ(10U, list->capacity);
	ASSERT_EQ(4U, list->size);
	ArrayListImplTest_checkContents(list, 7);

	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestRemoveMultipleReallocation)
{
	size_t initialCapacity = 10;
	unsigned int removeThreshold = 4;
	unsigned int addRemoveMultiplier = 2;
	size_t maxSize = 160; // 4 reallocations of size 10 with a multiplier of 2
	
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		initialCapacity,
		addRemoveMultiplier,
		removeThreshold,
		addRemoveMultiplier
	);

	// Fill the list
	for (size_t i = 0; i < maxSize; ++i)
	{
		ArrayListImpl_add(list, &i);
	}

	ArrayListImplTest_checkContents(list, 0);
	ASSERT_EQ(maxSize, list->size);
	ASSERT_EQ(maxSize, list->capacity);

	// Remove elements up until the next reallocation (160 / 4 = 40)
	size_t i = 0;
	while (list->size > 40)
	{
		ArrayListImpl_remove(list, 0);
		ArrayListImplTest_checkContents(list, ++i);
		ASSERT_EQ(maxSize - i, list->size);
		ASSERT_EQ(maxSize, list->capacity);
	}

	// The following remove should reduce capacity to 80
	ArrayListImpl_remove(list, 0);
	ArrayListImplTest_checkContents(list, ++i);
	ASSERT_EQ(maxSize - i, list->size);
	ASSERT_EQ(80U, list->capacity);

	// Remove elements up until the next reallocation (80 / 4 = 20)
	while (list->size > 20)
	{
		ArrayListImpl_remove(list, 0);
		ArrayListImplTest_checkContents(list, ++i);
		ASSERT_EQ(maxSize - i, list->size);
		ASSERT_EQ(80U, list->capacity);
	}
	
	// The following remove should reduce capacity to 40
	ArrayListImpl_remove(list, 0);
	ArrayListImplTest_checkContents(list, ++i);
	ASSERT_EQ(maxSize - i, list->size);
	ASSERT_EQ(40U, list->capacity);
	
	// Remove elements up until the next reallocation (40 / 4 = 10)
	while (list->size > 10)
	{
		ArrayListImpl_remove(list, 0);
		ArrayListImplTest_checkContents(list, ++i);
		ASSERT_EQ(maxSize - i, list->size);
		ASSERT_EQ(40U, list->capacity);
	}
	
	// The following remove should reduce capacity to 20
	ArrayListImpl_remove(list, 0);
	ArrayListImplTest_checkContents(list, ++i);
	ASSERT_EQ(maxSize - i, list->size);
	ASSERT_EQ(20U, list->capacity);
	
	// Remove elements up until the next reallocation (20 / 4 = 5)
	while (list->size > 5)
	{
		ArrayListImpl_remove(list, 0);
		ArrayListImplTest_checkContents(list, ++i);
		ASSERT_EQ(maxSize - i, list->size);
		ASSERT_EQ(20U, list->capacity);
	}
	
	// The following remove should reduce capacity to 10
	ArrayListImpl_remove(list, 0);
	ArrayListImplTest_checkContents(list, ++i);
	ASSERT_EQ(maxSize - i, list->size);
	ASSERT_EQ(10U, list->capacity);
	
	// Remove the remaining elements
	while (list->size > 0)
	{
		ArrayListImpl_remove(list, 0);
		ArrayListImplTest_checkContents(list, ++i);
		ASSERT_EQ(maxSize - i, list->size);
		ASSERT_EQ(10U, list->capacity);
	}
	
	// The size should be 0, but the capacity should remain 10
	ArrayListImpl_remove(list, 0);
	ArrayListImplTest_checkContents(list, ++i);
	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(10U, list->capacity);

	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestIteratorNextOperations)
{
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	const size_t expectedSize = 50;	
	for (size_t i = 0; i < expectedSize; ++i)
	{
		ArrayListImpl_add(list, &i);
	}

	ArrayListImplIterator* iter = ArrayListImpl_iterator(list);
	ASSERT_TRUE(ArrayListImplIterator_hasNext(iter));
	ASSERT_EQ(0U, *(size_t*)ArrayListImplIterator_peekNext(iter));

	size_t times = 0;
	while(ArrayListImplIterator_hasNext(iter))
	{
		ASSERT_EQ(times, *((size_t*)ArrayListImplIterator_peekNext(iter)));
		ASSERT_EQ(times, iter->nextIndex);
		ASSERT_EQ(times, *((size_t*)ArrayListImplIterator_next(iter)));
		ASSERT_EQ(++times, iter->nextIndex);
	}

	ASSERT_FALSE(ArrayListImplIterator_hasNext(iter));
	ASSERT_EQ(NULL, ArrayListImplIterator_peekNext(iter));
	ASSERT_EQ(NULL, ArrayListImplIterator_next(iter));
	ASSERT_EQ(expectedSize, times);
	ASSERT_EQ(times, iter->nextIndex);

	ArrayListImplIterator_destroy(iter);
	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestIteratorEmptyList)
{
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);
	ArrayListImplIterator* iter = ArrayListImpl_iterator(list);
	
	ASSERT_FALSE(ArrayListImplIterator_hasNext(iter));
	ASSERT_EQ(NULL, ArrayListImplIterator_peekNext(iter));
	ASSERT_EQ(NULL, ArrayListImplIterator_next(iter));
	ASSERT_EQ(0U, iter->nextIndex);
	
	/*
 	* Many calls to remove on an "empty" iterator
 	* should have no effect on the list.
 	*/
	ArrayListImplIterator_remove(iter);
	ArrayListImplIterator_remove(iter);
	ArrayListImplIterator_remove(iter);

	ASSERT_FALSE(ArrayListImplIterator_hasNext(iter));
	ASSERT_EQ(NULL, ArrayListImplIterator_peekNext(iter));
	ASSERT_EQ(NULL, ArrayListImplIterator_next(iter));
	ASSERT_EQ(0U, iter->nextIndex);
	
	ArrayListImplIterator_destroy(iter);
	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestIteratorCreatedBeforeListModification)
{
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);
	ArrayListImplIterator* iter = ArrayListImpl_iterator(list);

	const size_t expectedSize = 50;	
	for (size_t i = 0; i < expectedSize; ++i)
	{
		ArrayListImpl_add(list, &i);
	}

	ArrayListTest_smokeTestIterator(iter, expectedSize);

	ArrayListImplIterator_destroy(iter);
	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestIteratorRemoveFirstElementRepeated)
{
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	const size_t listSize = 50;	
	for (size_t i = 0; i < listSize; ++i)
	{
		ArrayListImpl_add(list, &i);
	}

	ArrayListImplIterator* iter = ArrayListImpl_iterator(list);
	for (size_t i = 0; i < listSize; ++i)
	{
		ASSERT_EQ(listSize - i, list->size);
		ArrayListImplTest_checkContents(list, i);

		ArrayListImplIterator_next(iter);
		ArrayListImplIterator_remove(iter);

		ASSERT_EQ(listSize - i - 1, list->size);
		ArrayListImplTest_checkContents(list, i + 1);
	}

	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(10U, list->capacity);

	ArrayListImplIterator_destroy(iter);
	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestIteratorRemoveLastElementRepeated)
{
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);

	const size_t listSize = 50;	
	for (size_t i = 0; i < listSize; ++i)
	{
		ArrayListImpl_add(list, &i);
	}

	ArrayListImplIterator* iter = ArrayListImpl_iterator(list);
	while (ArrayListImplIterator_hasNext(iter))
	{
		ArrayListImplIterator_next(iter);
	}

	for (size_t i = 0; i < listSize; ++i)
	{
		ASSERT_FALSE(ArrayListImplIterator_hasNext(iter));
		
		ASSERT_EQ(listSize - i, list->size);
		ArrayListImplTest_checkContents(list);

		ArrayListImplIterator_remove(iter);

		ASSERT_EQ(listSize - i - 1, list->size);
		ArrayListImplTest_checkContents(list);
	}

	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(10U, list->capacity);

	ArrayListImplIterator_destroy(iter);
	ArrayListImpl_destroy(list);
}
