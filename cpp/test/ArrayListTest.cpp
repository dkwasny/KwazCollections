#include "gtest/gtest.h"
#include "ArrayList.hpp"

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

TEST(ArrayList, ICollection_TestConstructor) {
	ICollection* coll = new ArrayList(10, 2, 4, 2);
	ASSERT_EQ(0, coll->getSize());
	delete coll;
}

TEST(ArrayList, ICollection_TestAdd) {
	ICollection* coll = new ArrayList(10, 2, 4, 2);

	for (int i = 1; i <= 30; ++i) {
		coll->add(i);
		ASSERT_EQ(i, coll->getSize());
	}

	delete coll;
}

TEST(ArrayList, IList_TestConstructor) {
	IList* list = new ArrayList(10, 2, 4, 2);
	ASSERT_EQ(0, list->getSize());
	delete list;
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
	ASSERT_EQ(0, list.getSize());
	ArrayListTest_smokeTestAddRemove(list);
}

TEST(ArrayList, TestCustomConstructor) {
	ArrayList list = ArrayList(1,4,3,2);
	ASSERT_EQ(0, list.getSize());
	ArrayListTest_smokeTestAddRemove(list);
}

TEST(ArrayList, TestCustomConstructorZeroCapacity) {
	ArrayList list = ArrayList(0,4,3,2);
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

	int i = 0;
 	for (; i < 10; ++i) {
                list.add(i);
        }

        ASSERT_EQ(i, list.getSize());
        ArrayListTest_checkContents(list);
}

TEST(ArrayList, TestAddOneReallocation) {
	ArrayList list = ArrayList(10, 2, 4, 2);

	int i = 0;
	for (; i < 10; ++i) {
		list.add(i);
	}
	ArrayListTest_checkContents(list);

	list.add(i++); 

	ASSERT_EQ(i, list.getSize());
	ArrayListTest_checkContents(list);

	for (; i < 13; ++i) {
		list.add(i);
	}

	ASSERT_EQ(i, list.getSize());
	ArrayListTest_checkContents(list);
}

// Since I really cant test the internals here, this test
// pretty much boiled down to an arbitrary stress test
// that mimics the number of iterations done in the C tests.
TEST(ArrayList, TestAddStressTest) {
	ArrayList list = ArrayList(10, 2, 4, 2);
	unsigned int i = 0;
	unsigned int goal = 10;
	for (int iteration = 0; iteration < 10; ++iteration) {
		for (; i < goal; ++i) {
			list.add(i);
		}
		ASSERT_EQ(i, list.getSize());
		ArrayListTest_checkContents(list);
		goal = i * 2;
	}
}
