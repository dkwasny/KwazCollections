#include "gtest/gtest.h"
#include "ArrayList.h"
#include <cmath>

// Test helper methods
void ArrayListTest_checkContents(ArrayList* list) {
	size_t i = 0;
	for(i = 0; i < list->size; ++i) {
		ASSERT_EQ(i, list->values[i]);
	}	
}

// Actual test cases
TEST(ArrayList, TestDefaultConstructor) {
	ArrayList* list = ArrayList_createDefault();
	ASSERT_EQ(0, list->size);
	ASSERT_EQ(ARRAY_LIST_INITIAL_CAPACITY, list->capacity);
}

TEST(ArrayList, TestAddNoReallocation) {
	ArrayList* list = ArrayList_createDefault();
	int* oldArray = list->values;

	int numToAdd = list->capacity;
	int i = 0;
	for (; i < numToAdd; ++i) {
		ArrayList_add(list, i);
	}

	ASSERT_EQ(ARRAY_LIST_INITIAL_CAPACITY, list->capacity);
	ASSERT_EQ(list->capacity, list->size);
	ASSERT_EQ(oldArray, list->values);
	ArrayListTest_checkContents(list);
}

TEST(ArrayList, TestAddOneReallocation) {
	ArrayList* list = ArrayList_createDefault();
	int* oldArray = list->values;

	int numToAdd = list->capacity;
	int i = 0;
	for (i = 0; i < numToAdd; ++i) {
		ArrayList_add(list, i);
	}

	ASSERT_EQ(ARRAY_LIST_INITIAL_CAPACITY, list->capacity);
	ASSERT_EQ(list->capacity, list->size);
	ASSERT_EQ(oldArray, list->values);
	ArrayListTest_checkContents(list);
	
	ArrayList_add(list, i++);

	size_t expectedCapacity = ARRAY_LIST_INITIAL_CAPACITY 
		* ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER;
	ASSERT_EQ(expectedCapacity, list->capacity);
	ASSERT_NE(list->capacity, list->size);
	ASSERT_EQ(i, list->size);
	ASSERT_NE(oldArray, list->values);
	ArrayListTest_checkContents(list);

	oldArray = list->values;

	numToAdd = list->capacity;
	for (; i < numToAdd; ++i) {
		ArrayList_add(list, i);
	}

	ASSERT_EQ(list->capacity, list->size);
	ASSERT_EQ(expectedCapacity, list->capacity);
	ASSERT_EQ(oldArray, list->values);
	ArrayListTest_checkContents(list);
}

TEST(ArrayList, TestAddMultipleReallocation) {
	ArrayList* list = ArrayList_createDefault();
	
	int* oldArray = list->values;
	int i = 0;
	int iteration = 0;
	for (iteration = 0; iteration < 10; ++iteration) {
		int numToAdd = list->capacity;
		for (; i < numToAdd; ++i) {
			ArrayList_add(list, i);
		}

		int expectedCapacity = ARRAY_LIST_INITIAL_CAPACITY 
			* pow(ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER, iteration);
		ASSERT_EQ(expectedCapacity, list->capacity);
		ASSERT_EQ(list->capacity, list->size);
		ASSERT_EQ(oldArray, list->values);
		ArrayListTest_checkContents(list);

		ArrayList_add(list, i++);

		expectedCapacity = ARRAY_LIST_INITIAL_CAPACITY 
			* pow(ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER, iteration + 1);
		ASSERT_EQ(expectedCapacity, list->capacity);
		ASSERT_NE(list->capacity, list->size);
		ASSERT_EQ(i, list->size);
		ASSERT_NE(oldArray, list->values);
		oldArray = list->values;
		ArrayListTest_checkContents(list);
	}
}
