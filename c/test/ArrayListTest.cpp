#include "gtest/gtest.h"
#include "ArrayList.h"

TEST(ArrayList, helloWorld) {
	ArrayList* list = ArrayList_createDefault();
	int i = 0;
	for (i = 0; i < 100; ++i) {
		ArrayList_add(list, i);
	}

	ASSERT_EQ(100, list->size);
}
