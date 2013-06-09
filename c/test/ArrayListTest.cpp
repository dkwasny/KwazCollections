#include "gtest/gtest.h"
#include "ArrayList.h"
#include <cmath>
#include <cstdio>

// Test helper methods
void ArrayListTest_checkContents(ArrayList* list)
{
	size_t i = 0;
	for(i = 0; i < list->getSize(list); ++i)
	{
		ASSERT_EQ(i, list->get(list, i));
	}	
}

void ArrayListTest_smokeTest(ArrayList* list)
{
	for (int i = 0; i < 20; ++i)
	{
		list->add(list, i);
	}
	ArrayListTest_checkContents(list);
	for (int i = 0; i < 20; ++i)
	{
		list->remove(list, 0);
	}
}

// Actual Tests
TEST(ArrayList, ICollection_TestAdd)
{
	ICollection* collection = ArrayList_create(10, 2, 4, 2)->superType->superType;

	for (int i = 1; i <= 30; ++i)
	{
		collection->add(collection, i);
		ASSERT_EQ(i, collection->getSize(collection));
	}

	collection->destroy(collection);
}

TEST(ArrayList, IList_TestAdd)
{
	IList* list = ArrayList_create(10, 2, 4, 2)->superType;

	for (int i = 1; i <= 30; ++i)
	{
		list->add(list, i);
		ASSERT_EQ(i, list->getSize(list));
	}

	list->destroy(list);
}

TEST(ArrayList, IList_TestGet)
{
	IList* list = ArrayList_create(10, 2, 4, 2)->superType;
	
	for (int i = 0; i < 30; ++i)
	{
		list->add(list, i);
	}

	for (int i = 0; i < 30; ++i)
	{
		ASSERT_EQ(i, list->get(list, i));
	}

	list->destroy(list);
}

TEST(ArrayList, IList_TestRemove)
{
	IList* list = ArrayList_create(10, 2, 4, 2)->superType;
	
	for (int i = 0; i < 30; ++i)
	{
		list->add(list, i);
	}

	for (int i = 0; i < 30; ++i)
	{
		ASSERT_EQ(i, list->remove(list, 0));
		ASSERT_EQ(30-1-i, list->getSize(list));
	}

	list->destroy(list);
}

TEST(ArrayList, TestDefaultConstructor)
{
	ArrayList* list = ArrayList_createDefault();
	ASSERT_EQ(10, list->getCapacity(list));
	ASSERT_EQ(0, list->getSize(list));
	ArrayListTest_smokeTest(list);
	list->destroy(list);
}

TEST(ArrayList, TestCustomConstructor)
{
	ArrayList* list = ArrayList_create(1, 2, 3, 4);
	ASSERT_EQ(1, list->getCapacity(list));
	ASSERT_EQ(0, list->getSize(list));
	ArrayListTest_smokeTest(list);
	list->destroy(list);
}

TEST(ArrayList, TestCustomConstructorZeroCapacity)
{
	ArrayList* list = ArrayList_create(0, 2, 3, 4);
	ASSERT_EQ(0, list->getCapacity(list));
	ASSERT_EQ(0, list->getSize(list));
	ArrayListTest_smokeTest(list);
	list->destroy(list);
}

TEST(ArrayList, TestAddNoReallocation)
{
	ArrayList* list = ArrayList_create(10, 2, 4, 2);

	for (int i = 0; i < list->getCapacity(list); ++i)
	{
		list->add(list, i);
		ASSERT_EQ(i+1, list->getSize(list));
		ASSERT_EQ(10, list->getCapacity(list));
	}

	ArrayListTest_checkContents(list);
	list->destroy(list);
}

TEST(ArrayList, TestAddOneReallocation)
{
	ArrayList* list = ArrayList_create(10, 2, 4, 2);

	int i = 0;
	for (; i < list->getCapacity(list); ++i)
	{
		list->add(list, i);
	}

	ASSERT_EQ(10, list->getSize(list));
	ASSERT_EQ(list->getSize(list), list->getCapacity(list));
	ArrayListTest_checkContents(list);
	
	list->add(list, i++);

	ASSERT_NE(list->getCapacity(list), list->getSize(list));
	ASSERT_EQ(11, list->getSize(list));
	ASSERT_EQ(20, list->getCapacity(list));
	ArrayListTest_checkContents(list);

	for (; i < list->getCapacity(list); ++i)
	{
		list->add(list, i);
	}

	ASSERT_EQ(20, list->getSize(list));
	ASSERT_EQ(list->getSize(list), list->getCapacity(list));
	ArrayListTest_checkContents(list);
	list->destroy(list);
}

TEST(ArrayList, TestAddMultipleReallocation)
{
	ArrayList* list = ArrayList_create(10, 2, 4, 2);
	
	int i = 0;
	for (int iteration = 0; iteration < 10; ++iteration)
	{
		for (; i < list->getCapacity(list); ++i)
		{
			list->add(list, i);
		}

		int expectedCapacity = 10 * pow(2, iteration);
		ASSERT_EQ(expectedCapacity, list->getCapacity(list));
		ASSERT_EQ(list->getCapacity(list), list->getSize(list));	
		ASSERT_EQ(i, list->getSize(list));
		ArrayListTest_checkContents(list);

		list->add(list, i++);

		expectedCapacity = 10 * pow(2, iteration + 1);
		ASSERT_EQ(expectedCapacity, list->getCapacity(list));
		ASSERT_NE(list->getCapacity(list), list->getSize(list));	
		ASSERT_EQ(i, list->getSize(list));
		ArrayListTest_checkContents(list);
	}
	list->destroy(list);
}

TEST(ArrayList, TestRemoveNoReallocation) {
	ArrayList* list = ArrayList_create(10, 2, 4, 2);

	for (int i = 0; i < list->getCapacity(list); ++i)
	{
		list->add(list, i);
	}

	ASSERT_EQ(10, list->getSize(list));
	ASSERT_EQ(list->getSize(list), list->getCapacity(list));

	int oldSize = list->getSize(list);
	for (int i = 0; i < oldSize;)
	{
		ASSERT_EQ(i++, list->remove(list, 0));
		ASSERT_EQ(oldSize-i, list->getSize(list));
		ASSERT_EQ(oldSize, list->getCapacity(list));
		for (int j = 0; j < list->getSize(list); ++j)
		{
			ASSERT_EQ(i+j, list->get(list, j));
		}
	}

	ASSERT_EQ(0, list->getSize(list));

	list->destroy(list);
}

TEST(ArrayList, TestRemoveOneReallocation) {
	ArrayList* list = ArrayList_create(10, 2, 4, 2);

	int i = 0;
	int currCapacity = list->getCapacity(list);
	for (int i = 0; i < currCapacity + 1; ++i) {
		list->add(list, i);
	}

	int oldSize = list->getSize(list);
	for (int i = 0; i < 6;) {
		ASSERT_EQ(i++, list->remove(list, 0));
		ASSERT_EQ(oldSize-i, list->getSize(list));
		ASSERT_EQ(20, list->getCapacity(list));
		for (int j = 0; j < list->getSize(list); ++j)
		{
			ASSERT_EQ(i+j, list->get(list, j));
		}
	}
	
	int oldCapacity = list->getCapacity(list);
	list->remove(list, 0);

	ASSERT_NE(oldCapacity, list->getCapacity(list));
	ASSERT_EQ(10, list->getCapacity(list));
	ASSERT_EQ(4, list->getSize(list));

	for (int i = 0; i < 4; ++i)
	{
		int expected = i + 7;
		ASSERT_EQ(expected, list->get(list, i));
	}

	list->destroy(list);
}

TEST(ArrayList, TestRemoveMultipleReallocation) {
}
