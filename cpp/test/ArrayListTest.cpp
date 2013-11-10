#include "gtest/gtest.h"
#include "ArrayList.hpp"
#include <cmath>

using KwazCollections::ICollection;
using KwazCollections::IList;
using KwazCollections::ArrayList;
using KwazCollections::IIterator;

// Helper methods
static void ArrayListTest_checkContents(IList<size_t>& list, size_t offset)
{
	for(size_t i = 0; i < list.getSize(); ++i)
	{
		ASSERT_EQ((size_t)(offset + i), list[i]);
	}
}

static void ArrayListTest_checkContents(IList<size_t>& list)
{
	ArrayListTest_checkContents(list, 0);	
}

static void ArrayListTest_smokeTestAdd(IList<size_t>& list)
{
	for (size_t i = 0; i < 20; ++i)
	{
		list.add(i);
	}
	ArrayListTest_checkContents(list);
}

static void ArrayListTest_smokeTestAddRemove(IList<size_t>& list)
{
	ArrayListTest_smokeTestAdd(list);
	for (size_t i = 0; i < 20; ++i)
	{
		list.remove(0);
	}
}

// Actual Tests
TEST(ArrayList, TestIListCopyConstructor)
{
	IList<size_t>* list = new ArrayList<size_t>(10, 2, 4, 2);
	ArrayListTest_smokeTestAdd(*list);

	ArrayList<size_t>* other = new ArrayList<size_t>(*list);

	//I have no idea if this is a valid assertion...
	ASSERT_NE(((ArrayList<size_t>*)list), other);

	ASSERT_EQ(list->getSize(), other->getSize());
	for (size_t i = 0; i < list->getSize(); ++i)
	{
		ASSERT_EQ((*list)[i], (*other)[i]);
	}

	delete other;
	delete list;
}

TEST(ArrayList, TestDefaultConstructor)
{
	ArrayList<size_t> list = ArrayList<size_t>();
	ASSERT_EQ(10U, list.getCapacity());
	ASSERT_EQ(0U, list.getSize());
	ArrayListTest_smokeTestAddRemove(list);
}

TEST(ArrayList, TestCustomConstructor)
{
	ArrayList<size_t> list = ArrayList<size_t>(1,4,3,2);
	ASSERT_EQ(1U, list.getCapacity());
	ASSERT_EQ(0U, list.getSize());
	ArrayListTest_smokeTestAddRemove(list);
}

TEST(ArrayList, TestCustomConstructorZeroCapacity)
{
	ArrayList<size_t> list = ArrayList<size_t>(0,4,3,2);
	ASSERT_EQ(0U, list.getCapacity());
	ASSERT_EQ(0U, list.getSize());
	ArrayListTest_smokeTestAddRemove(list);
}

TEST(ArrayList, TestCopyConstructor)
{
	ArrayList<size_t> list = ArrayList<size_t>(10, 2, 4, 2);
	ArrayListTest_smokeTestAdd(list);

	ArrayList<size_t> other = ArrayList<size_t>(list);
	ASSERT_NE(&list, &other);	

	ASSERT_EQ(list.getSize(), other.getSize());
	for (size_t i = 0; i < list.getSize(); ++i)
	{
		ASSERT_EQ(list[i], other[i]);
	}
}

TEST(ArrayList, TestAssignmentOperator)
{
	ArrayList<size_t> list = ArrayList<size_t>(10, 2, 4, 2);
	ArrayListTest_smokeTestAdd(list);

	ArrayList<size_t> other = list;
	ASSERT_NE(&list, &other);

	ASSERT_EQ(list.getSize(), other.getSize());
	for (size_t i = 0; i < list.getSize(); ++i)
	{
		ASSERT_EQ(list[i], other[i]);
	}
}

TEST(ArrayList, TestAddNoReallocation)
{
	ArrayList<size_t> list = ArrayList<size_t>(10, 2, 4, 2);

 	for (size_t i = 0; i < 10; ++i)
	{
                list.add(i);
		ASSERT_EQ(i+1, list.getSize());
        	ASSERT_EQ(10U, list.getCapacity());
        }

        ArrayListTest_checkContents(list);
}

TEST(ArrayList, TestAddOneReallocation)
{
	ArrayList<size_t> list = ArrayList<size_t>(10, 2, 4, 2);

	size_t i = 0;
	for (; i < 10; ++i)
	{
		list.add(i);
	}

	ASSERT_EQ(10U, list.getSize());
        ASSERT_EQ(list.getSize(), list.getCapacity());
	ArrayListTest_checkContents(list);

	list.add(i++); 

	ASSERT_NE(list.getCapacity(), list.getSize());
	ASSERT_EQ(11U, list.getSize());
	ASSERT_EQ(20U, list.getCapacity());
	ArrayListTest_checkContents(list);

	for (; i < list.getCapacity(); ++i)
	{
		list.add(i);
	}

	ASSERT_EQ(20U, list.getSize());
	ASSERT_EQ(list.getSize(), list.getCapacity());
	ArrayListTest_checkContents(list);
}

TEST(ArrayList, TestAddMultipleReallocation)
{
	ArrayList<size_t> list = ArrayList<size_t>(10, 2, 4, 2);
	unsigned int i = 0;
	for (int iteration = 0; iteration < 10; ++iteration)
	{
		for (; i < list.getCapacity(); ++i)
		{
			list.add(i);
		}

		size_t expectedCapacity = 10 * pow(2, iteration);
		ASSERT_EQ(expectedCapacity, list.getCapacity());
		ASSERT_EQ(list.getCapacity(), list.getSize());	
		ASSERT_EQ(i, list.getSize());
		ArrayListTest_checkContents(list);

		list.add(i++);

		expectedCapacity = 10 * pow(2, iteration + 1);
		ASSERT_EQ(expectedCapacity, list.getCapacity());
		ASSERT_NE(list.getCapacity(), list.getSize());	
		ASSERT_EQ(i, list.getSize());
		ArrayListTest_checkContents(list);
	}
}

TEST(ArrayList, TestIterator)
{
	ArrayList<size_t> list = ArrayList<size_t>(10, 2, 4, 2);
	for (size_t i = 0; i < 50; ++i)
	{
		list.add(i);
	}

	IIterator<size_t>* iter = list.iterator();
	
	size_t i = 0;
	for(; iter->hasNext(); ++i)
	{
		ASSERT_EQ(i, iter->peekNext());
		ASSERT_EQ(i, iter->next());
	}
	ASSERT_EQ(50U, i);
	
	delete iter;
}

TEST(ArrayList, TestRemoveNoReallocation)
{
	ArrayList<size_t> list = ArrayList<size_t>(10, 2, 4, 2);

	for (size_t i = 0; i < list.getCapacity(); ++i)
	{
		list.add(i);
	}

	ASSERT_EQ(10U, list.getSize());
	ASSERT_EQ(list.getSize(), list.getCapacity());

	size_t oldSize = list.getSize();
	for (size_t i = 0; i < oldSize;)
	{
		ASSERT_EQ(i++, list.remove(0));
		ASSERT_EQ(oldSize-i, list.getSize());
		ASSERT_EQ(oldSize, list.getCapacity());
		ArrayListTest_checkContents(list, i);
	}

	ASSERT_EQ(0U, list.getSize());
}

TEST(ArrayList, TestRemoveOneReallocation)
{
	ArrayList<size_t> list = ArrayList<size_t>(10, 2, 4, 2);

	size_t currCapacity = list.getCapacity();
	for (size_t i = 0; i < currCapacity + 1; ++i)
	{
		list.add(i);
	}

	size_t oldSize = list.getSize();
	for (size_t i = 0; i < 6;)
	{
		ASSERT_EQ(i++, list.remove(0));
		ASSERT_EQ(oldSize-i, list.getSize());
		ASSERT_EQ(20U, list.getCapacity());
		ArrayListTest_checkContents(list, i);
	}
	
	size_t oldCapacity = list.getCapacity();
	list.remove(0);

	ASSERT_NE(oldCapacity, list.getCapacity());
	ASSERT_EQ(10U, list.getCapacity());
	ASSERT_EQ(4U, list.getSize());
	ArrayListTest_checkContents(list, 7);
}

TEST(ArrayList, TestRemoveMultipleReallocation)
{
	size_t initialCapacity = 10;
	unsigned int removeThreshold = 4;
	unsigned int addRemoveMultiplier = 2;
	size_t maxSize = 160; // 4 reallocations of size 10 with a multiplier of 2
	
	ArrayList<size_t> list = ArrayList<size_t>(
		initialCapacity,
		addRemoveMultiplier,
		removeThreshold,
		addRemoveMultiplier
	);

	// Fill the list
	for (size_t i = 0; i < maxSize; ++i)
	{
		list.add(i);
	}

	ArrayListTest_checkContents(list, 0);
	ASSERT_EQ(maxSize, list.getSize());
	ASSERT_EQ(maxSize, list.getCapacity());

	// Remove elements up until the next reallocation (160 / 4 = 40)
	size_t i = 0;
	while (list.getSize() > 40)
	{
		list.remove(0);
		ArrayListTest_checkContents(list, ++i);
		ASSERT_EQ(maxSize - i, list.getSize());
		ASSERT_EQ(maxSize, list.getCapacity());
	}

	// The following remove should reduce capacity to 80
	list.remove(0);
	ArrayListTest_checkContents(list, ++i);
	ASSERT_EQ(maxSize - i, list.getSize());
	ASSERT_EQ(80U, list.getCapacity());

	// Remove elements up until the next reallocation (80 / 4 = 20)
	while (list.getSize() > 20)
	{
		list.remove(0);
		ArrayListTest_checkContents(list, ++i);
		ASSERT_EQ(maxSize - i, list.getSize());
		ASSERT_EQ(80U, list.getCapacity());
	}
	
	// The following remove should reduce capacity to 40
	list.remove(0);
	ArrayListTest_checkContents(list, ++i);
	ASSERT_EQ(maxSize - i, list.getSize());
	ASSERT_EQ(40U, list.getCapacity());
	
	// Remove elements up until the next reallocation (40 / 4 = 10)
	while (list.getSize() > 10)
	{
		list.remove(0);
		ArrayListTest_checkContents(list, ++i);
		ASSERT_EQ(maxSize - i, list.getSize());
		ASSERT_EQ(40U, list.getCapacity());
	}
	
	// The following remove should reduce capacity to 20
	list.remove(0);
	ArrayListTest_checkContents(list, ++i);
	ASSERT_EQ(maxSize - i, list.getSize());
	ASSERT_EQ(20U, list.getCapacity());
	
	// Remove elements up until the next reallocation (20 / 4 = 5)
	while (list.getSize() > 5)
	{
		list.remove(0);
		ArrayListTest_checkContents(list, ++i);
		ASSERT_EQ(maxSize - i, list.getSize());
		ASSERT_EQ(20U, list.getCapacity());
	}
	
	// The following remove should reduce capacity to 10
	list.remove(0);
	ArrayListTest_checkContents(list, ++i);
	ASSERT_EQ(maxSize - i, list.getSize());
	ASSERT_EQ(10U, list.getCapacity());
	
	// Remove the remaining elements
	while (list.getSize() > 0)
	{
		list.remove(0);
		ArrayListTest_checkContents(list, ++i);
		ASSERT_EQ(maxSize - i, list.getSize());
		ASSERT_EQ(10U, list.getCapacity());
	}
	
	// The size should be 0, but the capacity should remain 10
	ArrayListTest_checkContents(list, ++i);
	ASSERT_EQ(0U, list.getSize());
	ASSERT_EQ(10U, list.getCapacity());
}
