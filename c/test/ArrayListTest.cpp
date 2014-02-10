#include "gtest/gtest.h"
#include "ArrayList.h"
#include <cmath>
#include <cstdio>

// Test helper methods
static void ArrayListTest_checkContents(ArrayList* list)
{
	for(size_t i = 0; i < list->getSize(list); ++i)
	{
		ASSERT_EQ(i, *((size_t*)(list->get(list, i))));
	}	
}

static void ArrayListTest_smokeTest(ArrayList* list)
{
	for (size_t i = 0; i < 20; ++i)
	{
		list->add(list, &i);
	}
	ArrayListTest_checkContents(list);
	for (size_t i = 0; i < 20; ++i)
	{
		list->remove(list, 0);
	}
}

static void ArrayListTest_smokeTestIterator(IIterator* iter, size_t expectedElements)
{
	size_t i = 0;
	while (iter->hasNext(iter))
	{
		ASSERT_EQ(i, *((size_t*)(iter->peekNext(iter))));
		ASSERT_EQ(i, *((size_t*)(iter->next(iter))));
		++i;
	}

	ASSERT_EQ(NULL, iter->peekNext(iter));
	ASSERT_EQ(NULL, iter->next(iter));
	ASSERT_EQ(expectedElements, i);

	for (size_t i = 0; i < expectedElements; ++i)
	{
		iter->remove(iter);
	}
}

static ArrayList* ArrayListTest_createArrayList(size_t items)
{
	ArrayList* list = ArrayList_create(
		sizeof(size_t), 10, 2, 4, 2
	);

	for (size_t i = 0; i < items; ++i)
	{
		list->add(list, &i);
	}

	return list;
}

// Actual Tests
TEST(ArrayList, ICollection_TestAdd)
{
	ICollection* collection = ArrayList_create(
		sizeof(size_t), 10, 2, 4, 2
	)->superType->superType;

	for (size_t i = 1; i <= 30; ++i)
	{
		collection->add(collection, &i);
		ASSERT_EQ(i, collection->getSize(collection));
	}

	collection->destroy(collection);
}

TEST(ArrayList, ICollection_TestAddAll)
{
	ICollection* list = ArrayListTest_createArrayList(30)->superType->superType;
	ICollection* other = ArrayListTest_createArrayList(30)->superType->superType;

	list->addAll(list, other);
	ASSERT_EQ(60U, list->getSize(list));

	list->destroy(list);
	other->destroy(other);
}

TEST(ArrayList, ICollection_TestIterator)
{
	ICollection* collection = ArrayList_create(
		sizeof(size_t), 10, 2, 4, 2
	)->superType->superType;

	size_t i = 0;
	for (; i < 50; ++i)
	{
		collection->add(collection, &i);
	}

	IIterator* iter = collection->iterator(collection);
	ArrayListTest_smokeTestIterator(iter, 50);

	iter->destroy(iter);
	collection->destroy(collection);
}

TEST(ArrayList, IList_TestAdd)
{
	IList* list = ArrayList_create(
		sizeof(size_t), 10, 2, 4, 2
	)->superType;

	for (size_t i = 1; i <= 30; ++i)
	{
		list->add(list, &i);
		ASSERT_EQ(i, list->getSize(list));
	}

	list->destroy(list);
}

TEST(ArrayList, IList_TestAddAll)
{
	IList* list = ArrayListTest_createArrayList(30)->superType;
	ICollection* other = ArrayListTest_createArrayList(30)->superType->superType;

	list->addAll(list, other);

	ASSERT_EQ(60U, list->getSize(list));

	list->destroy(list);
	other->destroy(other);
}

TEST(ArrayList, IList_TestGet)
{
	IList* list = ArrayListTest_createArrayList(30)->superType;
	
	for (size_t i = 0; i < 30; ++i)
	{
		ASSERT_EQ(i, *((size_t*)(list->get(list, i))));
	}

	list->destroy(list);
}

TEST(ArrayList, IList_TestRemove)
{
	IList* list = ArrayListTest_createArrayList(30)->superType;
	
	for (size_t i = 0; i < 30; ++i)
	{
		list->remove(list,0);
		ASSERT_EQ(30-1-i, list->getSize(list));
	}

	list->destroy(list);
}

TEST(ArrayList, IList_TestIterator)
{
	IList* list = ArrayListTest_createArrayList(50)->superType;

	IIterator* iter = list->iterator(list);
	ArrayListTest_smokeTestIterator(iter, 50);

	iter->destroy(iter);
	list->destroy(list);
}

TEST(ArrayList, ArrayList_TestAdd)
{
	ArrayList* list = ArrayList_create(
		sizeof(size_t), 10, 2, 4, 2
	);

	for (size_t i = 1; i <= 30; ++i)
	{
		list->add(list, &i);
		ASSERT_EQ(i, list->getSize(list));
	}

	list->destroy(list);
}

TEST(ArrayList, ArrayList_TestAddAll)
{
	ArrayList* list = ArrayListTest_createArrayList(30);
	ICollection* other = ArrayListTest_createArrayList(30)->superType->superType;

	list->addAll(list, other);

	ASSERT_EQ(60U, list->getSize(list));
	ASSERT_EQ(80U, list->getCapacity(list));

	list->destroy(list);
	other->destroy(other);
}

TEST(ArrayList, ArrayList_TestIterator)
{
	ArrayList* list = ArrayListTest_createArrayList(50);

	IIterator* iter = list->iterator(list);
	ArrayListTest_smokeTestIterator(iter, 50);

	// Smoke test should clear the list with remove ops
	ASSERT_EQ(0U, list->impl->size);
	ASSERT_EQ(10U, list->impl->capacity);

	iter->destroy(iter);
	list->destroy(list);
}

TEST(ArrayList, ArrayList_TestGet)
{
	ArrayList* list = ArrayListTest_createArrayList(30);

	for (size_t i = 0; i < 30; ++i)
	{
		ASSERT_EQ(i, *((size_t*)(list->get(list, i))));
	}

	list->destroy(list);
}

TEST(ArrayList, ArrayList_TestRemove)
{
	ArrayList* list = ArrayListTest_createArrayList(30);

	for (size_t i = 0; i < 30; ++i)
	{
		list->remove(list, 0);
		ASSERT_EQ(30-1-i, list->getSize(list));
	}

	list->destroy(list);
}

TEST(ArrayList, ArrayList_TestGetCapacity)
{
	ArrayList* list = ArrayList_create(
		sizeof(size_t), 10, 2, 4, 2
	);

	size_t i = 0;	
	for (; i < 10; ++i)
	{
		list->add(list, &i);
		ASSERT_EQ(10U, list->getCapacity(list));
	}

	list->add(list, &++i);
	ASSERT_EQ(20U, list->getCapacity(list));

	list->destroy(list);
}

TEST(ArrayList, TestDefaultConstructor)
{
	ArrayList* list = ArrayList_createDefault(sizeof(size_t));
	ASSERT_EQ(10U, list->getCapacity(list));
	ASSERT_EQ(0U, list->getSize(list));
	ArrayListTest_smokeTest(list);
	list->destroy(list);
}

TEST(ArrayList, TestCustomConstructor)
{
	ArrayList* list = ArrayList_create(
		sizeof(size_t), 1, 2, 3, 4
	);
	ASSERT_EQ(1U, list->getCapacity(list));
	ASSERT_EQ(0U, list->getSize(list));
	ArrayListTest_smokeTest(list);
	list->destroy(list);
}
