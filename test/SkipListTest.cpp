#include "gtest/gtest.h"
#include "SkipList.h"
#include <stdio.h>

static void printList(SkipList* pList)
{

    // for (int i = 0; i < pList->numLevels && pList->size > 0; i++)
    SkipListNode* currHead = pList->topHead;
    while (true)
    {
        SkipListNode* currNode = currHead;
        while (true)
        {
            int nextVal = (currNode->next != NULL) ? currNode->next->value : -1;
            int downVal = (currNode->down != NULL) ? currNode->down->value : -1;
            printf("%d -> (%d, %d, %u)\n", currNode->value, nextVal, downVal, currNode->distNext);

            if (currNode->next != NULL)
            {
                currNode = currNode->next;
            }
            else
            {
                break;
            }

        }
        if (currHead->down != NULL)
        {
            currHead = currHead->down;
            printf("--\n");
        }
        else
        {
            break;
        }
    }
    printf("=====\n");
}

TEST(SkipList, IncreasingInsert)
{
    SkipList* list = SkipList_create();

    ASSERT_EQ(0U, list->size);

    for (int i = 0; i < 20; i++)
    {
        SkipList_add(list, i);
        printList(list);
    }

    ASSERT_EQ(20U, list->size);

    printList(list);

    SkipList_destroy(list);
}

TEST(SkipList, DecreasingInsert)
{
    SkipList* list = SkipList_create();

    ASSERT_EQ(0U, list->size);

    for (int i = 49; i >= 0; i--)
    {
        SkipList_add(list, i);
    }

    ASSERT_EQ(50U, list->size);

    printList(list);

    SkipList_add(list, 100);

    SkipList_destroy(list);
}

TEST(SkipList, AddNewLevelOnEmptyList)
{
    SkipList* list = SkipList_create();
    SkipList_addNewLevel(list);
    SkipList_destroy(list);
}

TEST(SkipList, IncreasingInsertPresized)
{
    SkipList* list = SkipList_createFull(5U, 10U);

    ASSERT_EQ(0U, list->size);

    for (int i = 0; i < 60; i++)
    {
        SkipList_add(list, i);
    }

    ASSERT_EQ(60U, list->size);

    printList(list);

    SkipList_destroy(list);
}

TEST(SkipList, Contains)
{
    SkipList* list = SkipList_create();

    ASSERT_EQ(0U, list->size);

    for (int i = 0; i < 10; i++)
    {
        SkipList_add(list, i * 2);
    }

    ASSERT_EQ(10U, list->size);

    printList(list);

    ASSERT_TRUE(SkipList_contains(list, 8));
    ASSERT_FALSE(SkipList_contains(list, 9));

    SkipList_destroy(list);
}
