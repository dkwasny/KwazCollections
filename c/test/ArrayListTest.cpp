#include "gtest/gtest.h"
#include "ArrayList.h"
#include <cmath>
#include <cstdio>

// Test helper methods
static void ArrayListTest_checkContents(ArrayList* list, int offset)
{
	for(size_t i = 0; i < list->getSize(list); ++i)
	{
		ASSERT_EQ(offset + i, list->get(list, i));
	}	
}

static void ArrayListTest_checkContents(ArrayList* list)
{
	ArrayListTest_checkContents(list, 0);
}

static void ArrayListTest_smokeTest(ArrayList* list)
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

static void ArrayListTest_smokeTestIterator(IIterator* iter, int expectedElements)
{
	int i = 0;
	while (iter->hasNext(iter))
	{
		ASSERT_EQ(i, iter->peekNext(iter));
		ASSERT_EQ(i, iter->next(iter));
		ASSERT_EQ(i, iter->current(iter));
		++i;
	}

	// Need to decrement to counteract the final increment done in the
	// previous loop.
	ASSERT_EQ(expectedElements, i--);

	while(iter->hasPrevious(iter))
	{
		--i;
		ASSERT_EQ(i, iter->peekPrevious(iter));
		ASSERT_EQ(i, iter->previous(iter));
		ASSERT_EQ(i, iter->current(iter));
	}

	ASSERT_EQ(0, i);
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

TEST(ArrayList, ICollection_TestIterator)
{
	ICollection* collection = ArrayList_create(10, 2, 4, 2)->superType->superType;

	for (int i = 0; i < 50; ++i)
	{
		collection->add(collection, i);
	}

	IIterator* iter = collection->iterator(collection);
	ArrayListTest_smokeTestIterator(iter, 50);

	iter->destroy(iter);
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

TEST(ArrayList, IList_TestIterator)
{
	IList* list = ArrayList_create(10, 2, 4, 2)->superType;

	for (int i = 0; i < 50; ++i)
	{
		list->add(list, i);
	}

	IIterator* iter = list->iterator(list);
	ArrayListTest_smokeTestIterator(iter, 50);

	iter->destroy(iter);
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

//TODO: Smoke test all of the ArrayList interface methods...why arent they here again??
