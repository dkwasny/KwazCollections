#include "gtest/gtest.h"
#include "ArrayList.hpp"
#include <cmath>

void ArrayListTest_checkContents(IList& list) {
	for (int i = 0; i < list.getSize(); ++i) {
		ASSERT_EQ(i, list[i]);
	}
}

void ArrayListTest_smokeTestAdd(IList& list) {
	for (int i = 0; i < 20; ++i) {
		list.add(i);
	}
	ArrayListTest_checkContents(list);
}

void ArrayListTest_smokeTestAddRemove(IList& list) {
	ArrayListTest_smokeTestAdd(list);
	for (int i = 0; i < 20; ++i) {
		list.remove(0);
	}
}

TEST(ArrayList, ICollection_TestAdd) {
	ICollection* coll = new ArrayList(10, 2, 4, 2);

	for (int i = 1; i <= 30; ++i) {
		coll->add(i);
		ASSERT_EQ(i, coll->getSize());
	}

	delete coll;
}

TEST(ArrayList, IList_TestAdd) {
	IList* list = new ArrayList(10, 2, 4, 2);

	for (int i = 1; i <= 30; ++i) {
		list->add(i);
		ASSERT_EQ(i, list->getSize());
	}

	delete list;
}

TEST(ArrayList, IList_TestGet) {
	IList* list = new ArrayList(10, 2, 4, 2);

	for (int i = 0; i < 30; ++i) {
		list->add(i);
	}

	for (int i = 0; i < list->getSize(); ++i) {
		ASSERT_EQ(i, list->get(i));
	}

	delete list;
}

TEST(ArrayList, IList_TestRemove) {
	IList* list = new ArrayList(10, 2, 4, 2);

	for (int i = 0; i < 30; ++i) {
		list->add(i);
	}

	for (int i = 0; i < list->getSize(); ++i) {
		ASSERT_EQ(i, list->remove(0));
	}

	delete list;
}

TEST(ArrayList, TestIListCopyConstructor) {
	IList* list = new ArrayList(10, 2, 4, 2);
	ArrayListTest_smokeTestAdd(*list);

	ArrayList* other = new ArrayList(*list);

	//I have no idea if this is a valid assertion...
	ASSERT_NE(((ArrayList*)list), other);

	ASSERT_EQ(list->getSize(), other->getSize());
	for (int i = 0; i < list->getSize(); ++i) {
		ASSERT_EQ((*list)[i], (*other)[i]);
	}

	delete other;
	delete list;
}


TEST(ArrayList, TestDefaultConstructor) {
	ArrayList list = ArrayList();
	ASSERT_EQ(10, list.getCapacity());
	ASSERT_EQ(0, list.getSize());
	ArrayListTest_smokeTestAddRemove(list);
}

TEST(ArrayList, TestCustomConstructor) {
	ArrayList list = ArrayList(1,4,3,2);
	ASSERT_EQ(1, list.getCapacity());
	ASSERT_EQ(0, list.getSize());
	ArrayListTest_smokeTestAddRemove(list);
}

TEST(ArrayList, TestCustomConstructorZeroCapacity) {
	ArrayList list = ArrayList(0,4,3,2);
	ASSERT_EQ(0, list.getCapacity());
	ASSERT_EQ(0, list.getSize());
	ArrayListTest_smokeTestAddRemove(list);
}

TEST(ArrayList, TestCopyConstructor) {
	ArrayList list = ArrayList(10, 2, 4, 2);
	ArrayListTest_smokeTestAdd(list);

	ArrayList other = ArrayList(list);
	ASSERT_NE(&list, &other);	

	ASSERT_EQ(list.getSize(), other.getSize());
	for (int i = 0; i < list.getSize(); ++i) {
		ASSERT_EQ(list[i], other[i]);
	}
}

TEST(ArrayList, TestAssignmentOperator) {
	ArrayList list = ArrayList(10, 2, 4, 2);
	ArrayListTest_smokeTestAdd(list);

	ArrayList other = list;
	ASSERT_NE(&list, &other);	

	ASSERT_EQ(list.getSize(), other.getSize());
	for (int i = 0; i < list.getSize(); ++i) {
		ASSERT_EQ(list[i], other[i]);
	}
}

TEST(ArrayList, TestAddNoReallocation) {
	ArrayList list = ArrayList(10, 2, 4, 2);

 	for (int i = 0; i < 10; ++i) {
                list.add(i);
		ASSERT_EQ(i+1, list.getSize());
        	ASSERT_EQ(10, list.getCapacity());
        }

        ArrayListTest_checkContents(list);
}

TEST(ArrayList, TestAddOneReallocation) {
	ArrayList list = ArrayList(10, 2, 4, 2);

	int i = 0;
	for (; i < 10; ++i) {
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

	for (; i < list.getCapacity(); ++i) {
		list.add(i);
	}

	ASSERT_EQ(20, list.getSize());
	ASSERT_EQ(list.getSize(), list.getCapacity());
	ArrayListTest_checkContents(list);
}

TEST(ArrayList, TestAddStressTest) {
	ArrayList list = ArrayList(10, 2, 4, 2);
	unsigned int i = 0;
	for (int iteration = 0; iteration < 10; ++iteration) {
		for (; i < list.getCapacity(); ++i) {
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
