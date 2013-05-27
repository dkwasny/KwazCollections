#include "gtest/gtest.h"
#include "ArrayList.hpp"

void ArrayListTest_checkContents(ArrayList& list) {
	for (int i = 0; i < list.getSize(); ++i) {
		ASSERT_EQ(i, list[i]);
	}
}

TEST(ArrayList, TestDefaultConstructor) {
	ArrayList list = ArrayList();
	ASSERT_EQ(0, list.getSize());
}

//TODO testother constructors

TEST(ArrayList, TestAddNoReallocation) {
	ArrayList list = ArrayList();

	int i = 0;
 	for (; i < ARRAY_LIST_INITIAL_CAPACITY; ++i) {
                list.add(i);
        }

        ASSERT_EQ(i, list.getSize());
        ArrayListTest_checkContents(list);
}

TEST(ArrayList, TestAddOneReallocation) {
	ArrayList list = ArrayList();

	int i = 0;
	for (; i < ARRAY_LIST_INITIAL_CAPACITY; ++i) {
		list.add(i);
	}
	ArrayListTest_checkContents(list);

	list.add(i++); 

	ASSERT_EQ(i, list.getSize());
	ArrayListTest_checkContents(list);

	for (; i < ARRAY_LIST_INITIAL_CAPACITY + 3; ++i) {
		list.add(i);
	}

	ASSERT_EQ(i, list.getSize());
	ArrayListTest_checkContents(list);
}

// Since I really cant test the internals here, this test
// pretty much boiled down to an arbitrary stress test
// that mimics the number of iterations done in the C tests.
TEST(ArrayList, TestAddMultipleReallocation) {
	ArrayList list = ArrayList();
	unsigned int i = 0;
	unsigned int goal = ARRAY_LIST_INITIAL_CAPACITY;
	for (int iteration = 0; iteration < 20; ++iteration) {
		for (; i < goal; ++i) {
			list.add(i);
		}
		ASSERT_EQ(i, list.getSize());
		ArrayListTest_checkContents(list);
		goal = i * ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER;
	}
}
