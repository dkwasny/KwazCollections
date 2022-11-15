#include "gtest/gtest.h"
#include "SkipList.h"
#include <stdio.h>

static void printList(SkipList* pList)
{

    for (int i = 0; i < pList->numLevels && pList->size > 0; i++)
    {
        SkipListNode* currNode = pList->heads + i;
        while (true)
        {
            int nextVal = (currNode->next != NULL) ? currNode->next->value : -1;
            int downVal = (currNode->down != NULL) ? currNode->down->value : -1;
            printf("%d -> (%d, %d)\n", currNode->value, nextVal, downVal);

            if (currNode->next != NULL)
            {
                currNode = currNode->next;
            }
            else
            {
                break;
            }

        }
        printf("--\n");
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
        SkipList_add(list, i);
        SkipList_add(list, i);
    }

    ASSERT_EQ(60U, list->size);

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
