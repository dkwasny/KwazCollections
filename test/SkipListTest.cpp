#include "gtest/gtest.h"
#include "SkipList.h"
#include <stdio.h>

static void printList(SkipList* pList)
{
    SkipListNode* currHead = pList->topHead;
    while (currHead != NULL)
    {
        SkipListNode* currNode = currHead;
        while (currNode != NULL)
        {
            int nextVal = (currNode->next != NULL) ? currNode->next->value : -1;
            int downVal = (currNode->down != NULL) ? currNode->down->value : -1;
            printf("%d -> (%d, %d, %u)\n", currNode->value, nextVal, downVal, currNode->distNext);

            currNode = currNode->next;
        }
        printf("--\n");
        currHead = currHead->down;
    }
    printf("=====\n");
}

static void verifyList(SkipList* pList)
{
    SkipListNode* currHead = pList->topHead;
    unsigned int numBottomNodes = 0U;
    while (currHead != NULL)
    {
        numBottomNodes = 0U;
        int expectedVal = 0;
        SkipListNode* currNode = currHead;
        while (currNode != NULL)
        {
            int nextVal = (currNode->next != NULL) ? currNode->next->value : -1;
            int downVal = (currNode->down != NULL) ? currNode->down->value : -1;

            ASSERT_EQ(expectedVal, currNode->value);
            if (downVal >= 0)
            {
                ASSERT_EQ(currNode->value, downVal);
            }

            if (currNode->next != NULL)
            {
                int expectedDist = currNode->next->value - currNode->value;
                ASSERT_EQ(expectedDist, currNode->distNext);
            }
            else
            {
                ASSERT_EQ(0U, currNode->distNext);
            }

            expectedVal += currNode->distNext;
            currNode = currNode->next;
            numBottomNodes++;
        }

        currHead = currHead->down;
    }

    ASSERT_EQ(numBottomNodes, pList->size);
}

static void mixedInsert(SkipList* pList, int numToInsert)
{
    int middle = numToInsert / 2;
    middle = (middle % 2 == 0) ? middle : middle + 1;
    for (int i = middle; i < numToInsert; i+=2)
    {
        SkipList_add(pList, i);
    }
    for (int i = middle - 2; i >= 0; i-=2)
    {
        SkipList_add(pList, i);
    }
    for (int i = 1; i < numToInsert; i+=2)
    {
        SkipList_add(pList, i);
    }
}

TEST(SkipList, Create)
{
    SkipList* list = SkipList_create();
    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(1U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);
    ASSERT_EQ(NULL, list->topHead);
    SkipList_destroy(list);
}

TEST(SkipList, CreateFull)
{
    SkipList* list = SkipList_createFull(20U, 30U, 60U);
    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(20U, list->numLevels);
    ASSERT_EQ(30U, list->maxLevels);
    ASSERT_EQ(60U, list->nextLevelChance);
    ASSERT_EQ(NULL, list->topHead);
    SkipList_destroy(list);
}

TEST(SkipList, CreateFullInvalidNumLevels)
{
    SkipList* list = SkipList_createFull(0U, 30U, 50U);
    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(1U, list->numLevels);
    ASSERT_EQ(30U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);
    ASSERT_EQ(NULL, list->topHead);
    SkipList_destroy(list);
}

TEST(SkipList, CreateFullInvalidMaxLevels)
{
    SkipList* list = SkipList_createFull(10U, 5U, 60U);
    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(10U, list->numLevels);
    ASSERT_EQ(10U, list->maxLevels);
    ASSERT_EQ(60U, list->nextLevelChance);
    ASSERT_EQ(NULL, list->topHead);
    SkipList_destroy(list);
}

TEST(SkipList, CreateFullInvalidNextLevelChance)
{
    SkipList* list = SkipList_createFull(10U, 20U, 110U);
    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(10U, list->numLevels);
    ASSERT_EQ(20U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);
    ASSERT_EQ(NULL, list->topHead);
    SkipList_destroy(list);
}

TEST(SkipList, IncreasingInsertSmall)
{
    SkipList* list = SkipList_create();
    for (int i = 0; i < 100; i++)
    {
        SkipList_add(list, i);
    }

    ASSERT_EQ(100U, list->size);
    ASSERT_EQ(6U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);
    verifyList(list);

    SkipList_destroy(list);
}

TEST(SkipList, IncreasingInsert)
{
    SkipList* list = SkipList_create();
    for (int i = 0; i < 1000; i++)
    {
        SkipList_add(list, i);
    }

    ASSERT_EQ(1000U, list->size);
    ASSERT_EQ(9U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);
    verifyList(list);

    SkipList_destroy(list);
}

TEST(SkipList, DecreasingInsert)
{
    SkipList* list = SkipList_create();
    for (int i = 999; i >= 0; i--)
    {
        SkipList_add(list, i);
    }

    ASSERT_EQ(1000U, list->size);
    ASSERT_EQ(9U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);

    verifyList(list);

    SkipList_destroy(list);
}

TEST(SkipList, ZipperInsert)
{
    SkipList* list = SkipList_create();
    for (int i = 0; i < 1000; i+=2)
    {
        SkipList_add(list, i);
    }
    for (int i = 1; i < 1000; i+=2)
    {
        SkipList_add(list, i);
    }

    ASSERT_EQ(1000U, list->size);
    ASSERT_EQ(9U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);
    verifyList(list);

    SkipList_destroy(list);
}

TEST(SkipList, MixedInsertDefaultChance)
{
    SkipList* list = SkipList_create();
    mixedInsert(list, 1000U);

    ASSERT_EQ(1000U, list->size);
    ASSERT_EQ(9U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);
    verifyList(list);

    SkipList_destroy(list);
}

TEST(SkipList, MixedInsertDefaultChanceLarge)
{
    SkipList* list = SkipList_create();
    mixedInsert(list, 100000U);

    ASSERT_EQ(100000U, list->size);
    ASSERT_EQ(16U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);
    verifyList(list);

    SkipList_destroy(list);
}


TEST(SkipList, MixedInsertNoLevelChance)
{
    SkipList* list = SkipList_createFull(1U, 40U, 0U);
    mixedInsert(list, 1000U);

    ASSERT_EQ(1000U, list->size);
    ASSERT_EQ(9U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(0U, list->nextLevelChance);
    verifyList(list);

    SkipList_destroy(list);
}

TEST(SkipList, MixedInsertVeryLowNextLevelChance)
{
    SkipList* list = SkipList_createFull(1U, 40U, 10U);
    mixedInsert(list, 1000U);

    ASSERT_EQ(1000U, list->size);
    ASSERT_EQ(9U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(10U, list->nextLevelChance);
    verifyList(list);

    SkipList_destroy(list);
}

TEST(SkipList, MixedInsertVeryHighLevelChance)
{
    SkipList* list = SkipList_createFull(1U, 40U, 90U);
    mixedInsert(list, 1000U);

    ASSERT_EQ(1000U, list->size);
    ASSERT_EQ(9U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(90U, list->nextLevelChance);
    verifyList(list);

    SkipList_destroy(list);
}

TEST(SkipList, MixedInsertAlwaysLevelChance)
{
    SkipList* list = SkipList_createFull(1U, 40U, 100U);
    mixedInsert(list, 1000U);

    ASSERT_EQ(1000U, list->size);
    ASSERT_EQ(9U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(100U, list->nextLevelChance);
    verifyList(list);

    SkipList_destroy(list);
}

TEST(SkipList, MixedInsertPresized)
{
    SkipList* list = SkipList_createFull(30U, 55U, 50U);
    mixedInsert(list, 1000U);

    ASSERT_EQ(1000U, list->size);
    ASSERT_EQ(30U, list->numLevels);
    ASSERT_EQ(55U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);
    verifyList(list);

    SkipList_destroy(list);
}

/*
    Verifying lists with duplicates is tricky.
    I can't easily rely on distance when verifying the values in the upper levels.
    I'm doing some rough checks for now until I care to harden this case more.
*/
TEST(SkipList, MixedInsertWithDuplicates)
{
    SkipList* list = SkipList_create();

    for (int i = 0; i < 100; i++)
    {
        mixedInsert(list, 100U);
    }

    ASSERT_EQ(10000U, list->size);
    ASSERT_EQ(13U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);

    SkipListNode* currHead = list->topHead;
    while (currHead->down != NULL)
    {
        int previousValue = currHead->value;
        SkipListNode* currNode = currHead->next;
        ASSERT_TRUE(currNode != NULL);
        while (currNode != NULL)
        {
            int currValue = currNode->value;
            ASSERT_LE(previousValue, currValue);
            previousValue = currValue;
            currNode = currNode->next;
        }

        currHead = currHead->down;
    }

    ASSERT_EQ(NULL, currHead->down);

   for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            ASSERT_EQ(i, currHead->value);
            currHead = currHead->next;
        }
    }

    ASSERT_EQ(NULL, currHead);

    SkipList_destroy(list);
}

TEST(SkipList, Contains)
{
    SkipList* list = SkipList_create();
    for (int i = 0; i < 1000; i+=2)
    {
        SkipList_add(list, i);
    }

    for (int i = 0; i < 1000; i+=2)
    {
        ASSERT_TRUE(SkipList_contains(list, i));
    }
    for (int i = 1; i < 1000; i+=2)
    {
        ASSERT_FALSE(SkipList_contains(list, i));
    }

    ASSERT_EQ(500U, list->size);
    ASSERT_EQ(8U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);

    SkipList_destroy(list);
}

TEST(SkipList, ContainsOnEmptyList)
{
    SkipList* list = SkipList_create();

    ASSERT_FALSE(SkipList_contains(list, 10));
    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(1U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);

    SkipList_destroy(list);
}

TEST(SkipList, AddNewLevelOnEmptyList)
{
    SkipList* list = SkipList_create();
    for (int i = 0; i < 10; i++)
    {
        SkipList_addNewLevel(list);
    }

    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(11U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);

    SkipList_destroy(list);
}

TEST(SkipList, Get)
{
    SkipList* list = SkipList_create();
    mixedInsert(list, 1000U);

    for (int i = 0; i < 1000; i++)
    {
        int actual = SkipList_get(list, i);
        ASSERT_EQ(i, actual);
    }

    ASSERT_EQ(1000U, list->size);
    ASSERT_EQ(9U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);

    SkipList_destroy(list);
}

TEST(SkipList, RemoveFromEmptyList)
{
    SkipList* list = SkipList_create();
    ASSERT_FALSE(SkipList_remove(list, 5));

    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(1U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);
    ASSERT_EQ(NULL, list->topHead);

    SkipList_destroy(list);
}

TEST(SkipList, RemoveFromSingleElementList)
{
    SkipList* list = SkipList_create();
    SkipList_add(list, 5);
    ASSERT_TRUE(SkipList_remove(list, 5));

    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(1U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);
    ASSERT_EQ(NULL, list->topHead);

    SkipList_destroy(list);
}

TEST(SkipList, RemoveFromMiddleOfList)
{
    SkipList* list = SkipList_create();
    mixedInsert(list, 10U);
    ASSERT_TRUE(SkipList_remove(list, 5));

    ASSERT_EQ(9U, list->size);
    ASSERT_EQ(3U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);

    for (int i = 0; i < 5; i++)
    {
        ASSERT_TRUE(SkipList_contains(list, i));
    }
    for (int i = 6; i < 10; i++)
    {
        ASSERT_TRUE(SkipList_contains(list, i));
    }

    SkipList_destroy(list);
}

TEST(SkipList, RemoveFromEndOfList)
{
    SkipList* list = SkipList_create();
    mixedInsert(list, 10U);
    ASSERT_TRUE(SkipList_remove(list, 9));

    ASSERT_EQ(9U, list->size);
    ASSERT_EQ(3U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);

    verifyList(list);

    SkipList_destroy(list);
}

TEST(SkipList, RemoveNonExisting)
{
    SkipList* list = SkipList_create();
    mixedInsert(list, 10U);
    SkipList_add(list, 12);

    ASSERT_FALSE(SkipList_remove(list, 11));


    ASSERT_EQ(11U, list->size);
    ASSERT_EQ(3U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);

    for (int i = 0; i < 10; i++)
    {
        ASSERT_TRUE(SkipList_contains(list, i));
    }
    ASSERT_TRUE(SkipList_contains(list, 12));

    SkipList_destroy(list);
}

TEST(SkipList, RemoveHead)
{
    SkipList* list = SkipList_create();
    mixedInsert(list, 10U);
    ASSERT_TRUE(SkipList_remove(list, 0));

    ASSERT_EQ(9U, list->size);
    ASSERT_EQ(3U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);

    for (int i = 1; i < 10; i++)
    {
        ASSERT_TRUE(SkipList_contains(list, i));
    }

    SkipList_destroy(list);
}

TEST(SkipList, RemoveAfterEnd)
{
    SkipList* list = SkipList_create();
    mixedInsert(list, 10U);
    ASSERT_FALSE(SkipList_remove(list, 100));

    ASSERT_EQ(10U, list->size);
    ASSERT_EQ(3U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);

    verifyList(list);

    SkipList_destroy(list);
}

TEST(SkipList, MixedRemove)
{
    SkipList* list = SkipList_create();
    mixedInsert(list, 100000U);

    for (int i = 0; i < 100000; i += 2)
    {
        SkipList_remove(list, i);
    }

    for (int i = 0; i < 100000; i++)
    {
        if (i % 2)
        {
            ASSERT_TRUE(SkipList_contains(list, i));
        }
        else
        {
            ASSERT_FALSE(SkipList_contains(list, i));
        }
    }

    ASSERT_EQ(50000U, list->size);
    ASSERT_EQ(16U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);

    SkipList_destroy(list);
}

TEST(SkipList, RemoveAllFromFront)
{
    SkipList* list = SkipList_create();
    mixedInsert(list, 1000U);
    for (int i = 0; i < 1000; i++)
    {
        ASSERT_TRUE(SkipList_remove(list, i));
    }

    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(9U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);
    ASSERT_EQ(NULL, list->topHead);

    SkipList_destroy(list);
}

TEST(SkipList, RemoveAllFromBack)
{
    SkipList* list = SkipList_create();
    mixedInsert(list, 1000U);
    for (int i = 999; i >= 0; i--)
    {
        ASSERT_TRUE(SkipList_remove(list, i));
    }

    ASSERT_EQ(0U, list->size);
    ASSERT_EQ(9U, list->numLevels);
    ASSERT_EQ(40U, list->maxLevels);
    ASSERT_EQ(50U, list->nextLevelChance);
    ASSERT_EQ(NULL, list->topHead);

    SkipList_destroy(list);
}