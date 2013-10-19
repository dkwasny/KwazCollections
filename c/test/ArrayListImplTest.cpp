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
		0, 2, 3, 4
	);
	ASSERT_EQ(0U, list->capacity);
	ASSERT_EQ(0U, list->size);
	ASSERT_EQ(2U, list->addReallocationMultiplier);
	ASSERT_EQ(3U, list->removeReallocationThreshold);
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
	for (size_t i = 0; i < oldSize;)
	{
		size_t* removedVal = (size_t*)ArrayListImpl_remove(list, 0);
		ASSERT_EQ(i++, *removedVal);
		free(removedVal);
		ASSERT_EQ(oldSize-i, list->size);
		ASSERT_EQ(oldSize, list->capacity);
		ArrayListImplTest_checkContents(list, i);
	}

	ASSERT_EQ(0U, list->size);

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
	size_t reallocations = 8;
	size_t initialCapacity = 10;
	unsigned int removeThreshold = 4;
	unsigned int addRemoveMultiplier = 2;
	size_t maxSize = 10 * pow(2, reallocations);
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		initialCapacity,
		addRemoveMultiplier,
		removeThreshold,
		addRemoveMultiplier
	);
	
	for (size_t i = 0; i < maxSize; ++i)
	{
		ArrayListImpl_add(list, &i);
	}
	
	size_t currOffset = 0;
	size_t iteration = 0;
	while (list->size != 0)
	{
		size_t numToRemove = list->size - (list->capacity / removeThreshold);
		for (size_t i = 0; i < numToRemove; ++i)
		{
			size_t* valToRemove = (size_t*)ArrayListImpl_get(list, 0);
			ASSERT_EQ(currOffset++, *valToRemove);
			ArrayListImpl_remove(list, 0);
		}
		
		ArrayListImplTest_checkContents(list, currOffset);
		ASSERT_EQ(maxSize - currOffset, list->size);
		// Do not forget that ArrayListImpls do not resize below their initial capacity...
		if (list->capacity != initialCapacity)
		{
			ASSERT_EQ(
				initialCapacity * pow(addRemoveMultiplier, reallocations - iteration),
				list->capacity
			);
		}

		size_t* valToRemove = (size_t*)ArrayListImpl_get(list, 0);
		ASSERT_EQ(currOffset++, *valToRemove);
		ArrayListImpl_remove(list, 0);
		
		ArrayListImplTest_checkContents(list, currOffset);
		ASSERT_EQ(maxSize - currOffset, list->size);
		// Do not forget that ArrayListImpls do not resize below their initial capacity...
		if (list->capacity != initialCapacity)
		{
			ASSERT_EQ(
				initialCapacity * pow(addRemoveMultiplier, reallocations - ++iteration),
				list->capacity
			);
		}
	}
	ArrayListImpl_destroy(list);
}

TEST(ArrayListImpl, TestIterator)
{
	ArrayListImpl* list = ArrayListImpl_create(
		sizeof(size_t),
		10, 2, 4, 2
	);
	ArrayListImplIterator* iter = ArrayListImpl_iterator(list);
	
	ASSERT_EQ(list, iter->list);
	ASSERT_EQ(0U, iter->nextIndex);

	ArrayListImplIterator_destroy(iter);
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
	ASSERT_EQ(0U, ArrayListImplIterator_peekNext(iter));

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
	ASSERT_EQ(times + 1, iter->nextIndex);

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

	const int expectedSize = 50;	
	for (int i = 0; i < expectedSize; ++i)
	{
		ArrayListImpl_add(list, &i);
	}

	ArrayListTest_smokeTestIterator(iter, expectedSize);

	ArrayListImplIterator_destroy(iter);
	ArrayListImpl_destroy(list);
}
