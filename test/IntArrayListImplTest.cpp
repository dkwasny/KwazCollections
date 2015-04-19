#include "gtest/gtest.h"
#include "IntArrayListImpl.h"
#include "Boolean.h"

TEST(IntArrayListImpl, GenericTest)
{
	IntArrayListImpl* list = IntArrayListImpl_create(
		ArrayListImpl_createDefault(sizeof(int))
	);
	
	for (int i = 0; i < 100; ++i)
	{
		IntArrayListImpl_add(list, i);
	}
	
	for (size_t i = 0; i < list->list->size; ++i)
	{
		ASSERT_EQ((int)i, IntArrayListImpl_get(list, i));
	}
	
	for (int i = 0; i < 100; ++i)
	{
		ASSERT_TRUE(IntArrayListImpl_contains(list, i));
	}
	
	for (size_t i = 0; i < 100; ++i)
	{
		IntArrayListImpl_remove(list, 0);
	}
	
	IntArrayListImpl_destroy(list);
}
