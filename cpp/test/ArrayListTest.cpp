#include "gtest/gtest.h"
#include "ArrayList.hpp"
#include <cmath>

using KwazCollections::ICollection;
using KwazCollections::IList;
using KwazCollections::ArrayList;
using KwazCollections::IIterator;

// Helper methods
static void ArrayListTest_checkContents(IList& list, int offset)
{
	for(size_t i = 0; i < list.getSize(); ++i)
	{
		ASSERT_EQ(offset + i, list[i]);
	}
}

static void ArrayListTest_checkContents(IList& list)
{
	ArrayListTest_checkContents(list, 0);	
}

static void ArrayListTest_smokeTestAdd(IList& list)
{
	for (int i = 0; i < 20; ++i)
	{
		list.add(i);
	}
	ArrayListTest_checkContents(list);
}

static void ArrayListTest_smokeTestAddRemove(IList& list)
{
	ArrayListTest_smokeTestAdd(list);
	for (int i = 0; i < 20; ++i)
	{
		list.remove(0);
	}
}

// Actual Tests
TEST(ArrayList, TestIListCopyConstructor)
{
	IList* list = new ArrayList(10, 2, 4, 2);
	ArrayListTest_smokeTestAdd(*list);

	ArrayList* other = new ArrayList(*list);

	//I have no idea if this is a valid assertion...
	ASSERT_NE(((ArrayList*)list), other);

	ASSERT_EQ(list->getSize(), other->getSize());
	for (int i = 0; i < list->getSize(); ++i)
	{
		ASSERT_EQ((*list)[i], (*other)[i]);
	}

	delete other;
	delete list;
}

TEST(ArrayList, TestDefaultConstructor)
{
	ArrayList list = ArrayList();
	ASSERT_EQ(10, list.getCapacity());
	ASSERT_EQ(0, list.getSize());
	ArrayListTest_smokeTestAddRemove(list);
}

TEST(ArrayList, TestCustomConstructor)
{
	ArrayList list = ArrayList(1,4,3,2);
	ASSERT_EQ(1, list.getCapacity());
	ASSERT_EQ(0, list.getSize());
	ArrayListTest_smokeTestAddRemove(list);
}

TEST(ArrayList, TestCustomConstructorZeroCapacity)
{
	ArrayList list = ArrayList(0,4,3,2);
	ASSERT_EQ(0, list.getCapacity());
	ASSERT_EQ(0, list.getSize());
	ArrayListTest_smokeTestAddRemove(list);
}

TEST(ArrayList, TestCopyConstructor)
{
	ArrayList list = ArrayList(10, 2, 4, 2);
	ArrayListTest_smokeTestAdd(list);

	ArrayList other = ArrayList(list);
	ASSERT_NE(&list, &other);	

	ASSERT_EQ(list.getSize(), other.getSize());
	for (int i = 0; i < list.getSize(); ++i)
	{
		ASSERT_EQ(list[i], other[i]);
	}
}

TEST(ArrayList, TestAssignmentOperator)
{
	ArrayList list = ArrayList(10, 2, 4, 2);
	ArrayListTest_smokeTestAdd(list);

	ArrayList other = list;
	ASSERT_NE(&list, &other);	

	ASSERT_EQ(list.getSize(), other.getSize());
	for (int i = 0; i < list.getSize(); ++i)
	{
		ASSERT_EQ(list[i], other[i]);
	}
}

TEST(ArrayList, TestAddNoReallocation)
{
	ArrayList list = ArrayList(10, 2, 4, 2);

 	for (int i = 0; i < 10; ++i)
	{
                list.add(i);
		ASSERT_EQ(i+1, list.getSize());
        	ASSERT_EQ(10, list.getCapacity());
        }

        ArrayListTest_checkContents(list);
}

TEST(ArrayList, TestAddOneReallocation)
{
	ArrayList list = ArrayList(10, 2, 4, 2);

	int i = 0;
	for (; i < 10; ++i)
	{
		list.add(i);
	}

	ASSERT_EQ(10, list.getSize());
        ASSERT_EQ(list.getSize(), list.getCapacity());
	ArrayListTest_checkContents(list);

	list.add(i++); 

	ASSERT_NE(list.getCapacity(), list.getSize());
	ASSERT_EQ(11, list.getSize());
	ASSERT_EQ(20, list.getCapacity());
	ArrayListTest_checkContents(list);

	for (; i < list.getCapacity(); ++i)
	{
		list.add(i);
	}

	ASSERT_EQ(20, list.getSize());
	ASSERT_EQ(list.getSize(), list.getCapacity());
	ArrayListTest_checkContents(list);
}

TEST(ArrayList, TestAddStressTest)
{
	ArrayList list = ArrayList(10, 2, 4, 2);
	unsigned int i = 0;
	for (int iteration = 0; iteration < 10; ++iteration)
	{
		for (; i < list.getCapacity(); ++i)
		{
			list.add(i);
		}

		int expectedCapacity = 10 * pow(2, iteration);
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
	ArrayList list = ArrayList(10, 2, 4, 2);
	for (size_t i = 0; i < 50; ++i)
	{
		list.add(i);
	}

	IIterator* iter = list.iterator();
	
	size_t i = 0;
	for(; iter->hasNext(); ++i)
	{
		ASSERT_EQ(i, iter->peekNext());
		ASSERT_EQ(i, iter->next());
	}
	ASSERT_EQ(50, i);
	
	delete iter;
}

TEST(ArrayList, TestRemoveNoReallocation)
{
	ArrayList list = ArrayList(10, 2, 4, 2);

	for (int i = 0; i < list.getCapacity(); ++i)
	{
		list.add(i);
	}

	ASSERT_EQ(10, list.getSize());
	ASSERT_EQ(list.getSize(), list.getCapacity());

	int oldSize = list.getSize();
	for (int i = 0; i < oldSize;)
	{
		ASSERT_EQ(i++, list.remove(0));
		ASSERT_EQ(oldSize-i, list.getSize());
		ASSERT_EQ(oldSize, list.getCapacity());
		ArrayListTest_checkContents(list, i);
	}

	ASSERT_EQ(0, list.getSize());
}

TEST(ArrayList, TestRemoveOneReallocation)
{
	ArrayList list = ArrayList(10, 2, 4, 2);

	int i = 0;
	int currCapacity = list.getCapacity();
	for (int i = 0; i < currCapacity + 1; ++i)
	{
		list.add(i);
	}

	int oldSize = list.getSize();
	for (int i = 0; i < 6;)
	{
		ASSERT_EQ(i++, list.remove(0));
		ASSERT_EQ(oldSize-i, list.getSize());
		ASSERT_EQ(20, list.getCapacity());
		ArrayListTest_checkContents(list, i);
	}
	
	int oldCapacity = list.getCapacity();
	list.remove(0);

	ASSERT_NE(oldCapacity, list.getCapacity());
	ASSERT_EQ(10, list.getCapacity());
	ASSERT_EQ(4, list.getSize());
	ArrayListTest_checkContents(list, 7);
}

TEST(ArrayList, TestRemoveMultipleReallocation)
{
	int reallocations = 8;
	int initialCapacity = 10;
	int removeThreshold = 4;
	int addRemoveMultiplier = 2;
	int maxSize = 10 * pow(2, reallocations);
	ArrayList list = ArrayList(
		initialCapacity,
		addRemoveMultiplier,
		removeThreshold,
		addRemoveMultiplier
	);
	
	for (int i = 0; i < maxSize; ++i)
	{
		list.add(i);
	}
	
	int currOffset = 0;
	int iteration = 0;
	while (list.getSize() != 0)
	{
		int numToRemove = list.getSize() - (list.getCapacity() / removeThreshold);
		for (int i = 0; i < numToRemove; ++i)
		{
			ASSERT_EQ(currOffset++, list.remove(0));
		}
		
		ArrayListTest_checkContents(list, currOffset);
		ASSERT_EQ(maxSize - currOffset, list.getSize());
		// Do not forget that ArrayLists do not resize below their initial capacity...
		if (list.getCapacity() != initialCapacity)
		{
			ASSERT_EQ(
				initialCapacity * pow(addRemoveMultiplier, reallocations - iteration),
				list.getCapacity()
			);
		}

		ASSERT_EQ(currOffset++, list.remove(0));
		
		ArrayListTest_checkContents(list, currOffset);
		ASSERT_EQ(maxSize - currOffset, list.getSize());
		// Do not forget that ArrayLists do not resize below their initial capacity...
		if (list.getCapacity() != initialCapacity)
		{
			ASSERT_EQ(
				initialCapacity * pow(addRemoveMultiplier, reallocations - ++iteration),
				list.getCapacity()
			);
		}
	}
}

/* This is a copy of an earlier test..if this is uncommented, bad_alloc will happen.
 * Look at ticket #17 on GitHub
TEST(ArrayList, TestIListCopyConstructorBadAllocCopy)
{
	IList* list = new ArrayList(10, 2, 4, 2);
	ArrayListTest_smokeTestAdd(*list);

	ArrayList* other = new ArrayList(*list);

	//I have no idea if this is a valid assertion...
	ASSERT_NE(((ArrayList*)list), other);

	ASSERT_EQ(list->getSize(), other->getSize());
	for (int i = 0; i < list->getSize(); ++i)
	{
		ASSERT_EQ((*list)[i], (*other)[i]);
	}

	delete other;
	delete list;
}
*/
