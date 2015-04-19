#include "gtest/gtest.h"
#include "IntArrayList.h"
#include "Boolean.h"

TEST(IntArrayList, GenericTest)
{
	IntArrayList* list = IntArrayList_create(
		ArrayList_createDefault(sizeof(int))
	);
	
	for (int i = 0; i < 100; ++i)
	{
		IntArrayList_add(list, i);
	}
	
	for (size_t i = 0; i < list->list->size; ++i)
	{
		ASSERT_EQ((int)i, IntArrayList_get(list, i));
	}
	
	for (int i = 0; i < 100; ++i)
	{
		ASSERT_TRUE(IntArrayList_contains(list, i));
	}
	
	for (size_t i = 0; i < 100; ++i)
	{
		IntArrayList_remove(list, 0);
	}
	
	IntArrayList_destroy(list);
}
