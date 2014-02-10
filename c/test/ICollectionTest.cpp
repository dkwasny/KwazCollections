#include "gtest/gtest.h"
#include "ArrayList.h"

/* I'm using ArrayList as the "implmentation" to test methods
 * provided by ICollection.
 */

static ArrayList* ICollection_createArrayList(size_t items)
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

TEST(ICollection, TestAddAll)
{
	ICollection* list = ICollection_createArrayList(30)->superType->superType;
	ICollection* other = ICollection_createArrayList(30)->superType->superType;

	list->addAll(list, other);

	ASSERT_EQ(60U, list->getSize(list));

	IIterator* iter = list->iterator(list);
	size_t i = 0;
	while (iter->hasNext(iter))
	{
		size_t expected = i++ % 30;
		ASSERT_EQ(expected, *((size_t*)(iter->next(iter))));
	}

	iter->destroy(iter);
	list->destroy(list);
	other->destroy(other);
}
