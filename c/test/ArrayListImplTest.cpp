#include "gtest/gtest.h"
#include "ArrayListImpl.h"

static void ArrayListImplTest_checkContents(ArrayListImpl* list, int offset)
{
	for(size_t i = 0; i < list->size; ++i)
	{
		ASSERT_EQ(offset + i, ArrayListImpl_get(list, i));
	}	
}

static void ArrayListImplTest_checkContents(ArrayListImpl* list)
{
	ArrayListImplTest_checkContents(list, 0);
}

static void ArrayListImplTest_smokeTest(ArrayListImpl* list)
{
	for (int i = 0; i < 20; ++i)
	{
		ArrayListImpl_add(list, i);
	}
	ArrayListImplTest_checkContents(list);
	for (int i = 0; i < 20; ++i)
	{
		ArrayListImpl_remove(list, 0);
	}
}

TEST(ArrayListImpl, TestDefaultConstructor)
{
	ArrayListImpl* list = ArrayListImpl_createDefault();
	ASSERT_EQ(10, list->capacity);
	ASSERT_EQ(0, list->size);
	ASSERT_EQ(2, list->addReallocationMultiplier);
	ASSERT_EQ(4, list->removeReallocationThreshold);
	ASSERT_EQ(2, list->removeReallocationDivisor);
	ArrayListImplTest_smokeTest(list);
	ArrayListImpl_delete(list);
}

TEST(ArrayListImpl, TestCustomConstructor)
{
	ArrayListImpl* list = ArrayListImpl_create(1, 5, 10, 5);
	ASSERT_EQ(1, list->capacity);
	ASSERT_EQ(0, list->size);
	ASSERT_EQ(5, list->addReallocationMultiplier);
	ASSERT_EQ(10, list->removeReallocationThreshold);
	ASSERT_EQ(5, list->removeReallocationDivisor);
	ArrayListImplTest_smokeTest(list);
	ArrayListImpl_delete(list);
}

TEST(ArrayListImpl, TestCustomConstructorZeroCapacity)
{
	ArrayListImpl* list = ArrayListImpl_create(0, 2, 3, 4);
	ASSERT_EQ(0, list->capacity);
	ASSERT_EQ(0, list->size);
	ArrayListImplTest_smokeTest(list);
	ArrayListImpl_delete(list);
}

TEST(ArrayListImpl, TestCustomConstructorRemoveThresholdLowerThanDivisor)
{
	ArrayListImpl* list = ArrayListImpl_create(10, 2, 4, 100);
	ASSERT_EQ(10, list->capacity);
	ASSERT_EQ(0, list->size);
	ASSERT_EQ(2, list->addReallocationMultiplier);
	ASSERT_EQ(200, list->removeReallocationThreshold);
	ASSERT_EQ(100, list->removeReallocationDivisor);
	ArrayListImplTest_smokeTest(list);
	ArrayListImpl_delete(list);
}

TEST(ArrayListImpl, TestAddNoReallocation)
{
	ArrayListImpl* list = ArrayListImpl_create(10, 2, 4, 2);

	for (int i = 0; i < list->capacity; ++i)
	{
		ArrayListImpl_add(list, i);
		ASSERT_EQ(i+1, list->size);
		ASSERT_EQ(10, list->capacity);
	}

	ArrayListImplTest_checkContents(list);
	ArrayListImpl_delete(list);
}

TEST(ArrayListImpl, TestAddOneReallocation)
{
	ArrayListImpl* list = ArrayListImpl_create(10, 2, 4, 2);

	int i = 0;
	for (; i < list->capacity; ++i)
	{
		ArrayListImpl_add(list, i);
	}

	ASSERT_EQ(10, list->size);
	ASSERT_EQ(list->size, list->capacity);
	ArrayListImplTest_checkContents(list);
	
	ArrayListImpl_add(list, i++);

	ASSERT_NE(list->capacity, list->size);
	ASSERT_EQ(11, list->size);
	ASSERT_EQ(20, list->capacity);
	ArrayListImplTest_checkContents(list);

	for (; i < list->capacity; ++i)
	{
		ArrayListImpl_add(list, i);
	}

	ASSERT_EQ(20, list->size);
	ASSERT_EQ(list->size, list->capacity);
	ArrayListImplTest_checkContents(list);
	ArrayListImpl_delete(list);
}

TEST(ArrayListImpl, TestAddMultipleReallocation)
{
	ArrayListImpl* list = ArrayListImpl_create(10, 2, 4, 2);
	
	int i = 0;
	for (int iteration = 0; iteration < 10; ++iteration)
	{
		for (; i < list->capacity; ++i)
		{
			ArrayListImpl_add(list, i);
		}

		int expectedCapacity = 10 * pow(2, iteration);
		ASSERT_EQ(expectedCapacity, list->capacity);
		ASSERT_EQ(list->capacity, list->size);	
		ASSERT_EQ(i, list->size);
		ArrayListImplTest_checkContents(list);

		ArrayListImpl_add(list, i++);

		expectedCapacity = 10 * pow(2, iteration + 1);
		ASSERT_EQ(expectedCapacity, list->capacity);
		ASSERT_NE(list->capacity, list->size);	
		ASSERT_EQ(i, list->size);
		ArrayListImplTest_checkContents(list);
	}
	ArrayListImpl_delete(list);
}

TEST(ArrayListImpl, TestRemoveNoReallocation)
{
	ArrayListImpl* list = ArrayListImpl_create(10, 2, 4, 2);

	for (int i = 0; i < list->capacity; ++i)
	{
		ArrayListImpl_add(list, i);
	}

	ASSERT_EQ(10, list->size);
	ASSERT_EQ(list->size, list->capacity);

	int oldSize = list->size;
	for (int i = 0; i < oldSize;)
	{
		ASSERT_EQ(i++, ArrayListImpl_remove(list, 0));
		ASSERT_EQ(oldSize-i, list->size);
		ASSERT_EQ(oldSize, list->capacity);
		ArrayListImplTest_checkContents(list, i);
	}

	ASSERT_EQ(0, list->size);

	ArrayListImpl_delete(list);
}

TEST(ArrayListImpl, TestRemoveOneReallocation)
{
	ArrayListImpl* list = ArrayListImpl_create(10, 2, 4, 2);

	int i = 0;
	int currCapacity = list->capacity;
	for (int i = 0; i < currCapacity + 1; ++i)
	{
		ArrayListImpl_add(list, i);
	}

	int oldSize = list->size;
	for (int i = 0; i < 6;)
	{
		ASSERT_EQ(i++, ArrayListImpl_remove(list, 0));
		ASSERT_EQ(oldSize-i, list->size);
		ASSERT_EQ(20, list->capacity);
		ArrayListImplTest_checkContents(list, i);
	}
	
	int oldCapacity = list->capacity;
	ArrayListImpl_remove(list, 0);

	ASSERT_NE(oldCapacity, list->capacity);
	ASSERT_EQ(10, list->capacity);
	ASSERT_EQ(4, list->size);
	ArrayListImplTest_checkContents(list, 7);

	ArrayListImpl_delete(list);
}

TEST(ArrayListImpl, TestRemoveMultipleReallocation)
{
	int reallocations = 8;
	int initialCapacity = 10;
	int removeThreshold = 4;
	int addRemoveMultiplier = 2;
	int maxSize = 10 * pow(2, reallocations);
	ArrayListImpl* list = ArrayListImpl_create(
		initialCapacity,
		addRemoveMultiplier,
		removeThreshold,
		addRemoveMultiplier
	);
	
	for (int i = 0; i < maxSize; ++i)
	{
		ArrayListImpl_add(list, i);
	}
	
	int currOffset = 0;
	int iteration = 0;
	while (list->size != 0)
	{
		int numToRemove = list->size - (list->capacity / removeThreshold);
		for (int i = 0; i < numToRemove; ++i)
		{
			ASSERT_EQ(currOffset++, ArrayListImpl_remove(list, 0));
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

		ASSERT_EQ(currOffset++, ArrayListImpl_remove(list, 0));
		
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
}
