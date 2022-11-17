#include "SkipList.h"
#include "Util.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static int randChance(void)
{
    return rand() % 100 < 50;
}

static unsigned int rollLevels(unsigned int numLevels)
{
    unsigned int retVal = 1;
    unsigned int i;
    Boolean passing = TRUE;
    for (i = 1; i < numLevels && passing; i++) {
        if (randChance())
        {
            printf("IMONANEWLEVEL!!\n");
            retVal++;
        }
        else
        {
            passing = FALSE;
        }
    }
    return retVal;
}

static void _SkipList_addNonHead(SkipList* pList, int pValue, Boolean dupesInBack)
{
    unsigned int i = pList->numLevels - 1;
    unsigned int numNewLevels = rollLevels(pList->numLevels);
    unsigned int* totalDistPerLevel = malloc(sizeof(unsigned int) * pList->numLevels);
    unsigned int totalDist = 0U;
    SkipListNode* currNode = pList->topHead;
    SkipListNode* prevNewNode = NULL;
    SkipListNode** closestNodes = malloc(sizeof(SkipListNode*) * pList->numLevels);
    memset(totalDistPerLevel, 0, sizeof(unsigned int) * pList->numLevels);
    while(1)
    {
        Boolean goRight = currNode->next != NULL
            && (
                pValue > currNode->next->value
                || (dupesInBack && pValue == currNode->next->value)
            );
        printf("TD: %u - TDPL: %u\n", totalDist, totalDistPerLevel[i]);
        if (goRight)
        {
            printf("RIGHT\n");
/*             totalDistPerLevel[i] += currNode->distNext; */
            totalDist += currNode->distNext;
            currNode = currNode->next;
        }
        else if (currNode->down != NULL)
        {
            printf("DOWN - %u\n", i);
            totalDistPerLevel[i] = totalDist;
            closestNodes[i--] = currNode;
            currNode = currNode->down;
        }
        else
        {
            printf("BREAK - %u\n", i);
            totalDistPerLevel[i] = totalDist;
            closestNodes[i] = currNode;
            break;
        }
    }

    printf("CLOSEST NODES\n");
    for (i = 0; i < pList->numLevels; i++)
    {
        printf("%u -> %u\n", i, totalDistPerLevel[i]);
    }

    printf("ADD - NEW LEVELS (%u)\n", numNewLevels);
    for (i = 0; i < numNewLevels; i++)
    {
        SkipListNode* newNode = malloc(sizeof(SkipListNode));
        newNode->value = pValue;
        newNode->next = closestNodes[i]->next;
        newNode->down = prevNewNode;
        if (closestNodes[i]->next == NULL)
        {
            unsigned int y = (totalDist - totalDistPerLevel[i]) + 1;
            closestNodes[i]->distNext = y;
            newNode->distNext = 0U;
        }
        else
        {
            unsigned int y = (totalDist - totalDistPerLevel[i]) + 1;
            unsigned int z = closestNodes[i]->distNext - (totalDist - totalDistPerLevel[i]);
            closestNodes[i]->distNext = y;
            newNode->distNext = z;
        }

        closestNodes[i]->next = newNode;

        prevNewNode = newNode;
    }

    free(totalDistPerLevel);
    free(closestNodes);
}

static void _SkipList_addHead(SkipList* pList, int pValue)
{
    SkipListNode* prevHead = NULL;
    SkipListNode* currHead = pList->topHead;
    SkipListNode* prevNewHead = NULL;
    SkipListNode* newTopHead = NULL;
    int oldHeadValue = currHead->value;

    while (currHead != NULL)
    {
        SkipListNode* newHead = malloc(sizeof(SkipListNode));
        newHead->value = pValue;
        newHead->next = currHead->next;
        newHead->distNext = currHead->distNext;
        newHead->down = NULL;
        if (newTopHead == NULL)
        {
            newTopHead = newHead;
        }

        if (prevNewHead != NULL)
        {
            prevNewHead->down = newHead;
        }
        prevNewHead = newHead;

        prevHead = currHead;
        currHead = currHead->down;
        free(prevHead);
    }

    pList->topHead = newTopHead;

    printf("READDING OLD HEAD\n");
    _SkipList_addNonHead(pList, oldHeadValue, FALSE);
}

SkipList* SkipList_create(void)
{
    return SkipList_createFull(1U, 5U);
}

SkipList* SkipList_createFull(const unsigned int pNumLevels, const unsigned int pMaxLevels)
{
    SkipList* retVal =  malloc(sizeof(SkipList));

    unsigned int numLevels = (pNumLevels < 1) ? 1U : pNumLevels;
    unsigned int maxLevels = (pMaxLevels < numLevels) ? numLevels : pMaxLevels;

    retVal->size = 0U;
    retVal->numLevels = numLevels;
    retVal->maxLevels = maxLevels;
    retVal->topHead = NULL;

    return retVal;
}

void SkipList_destroy(SkipList* pList)
{
    SkipListNode* prevHead = NULL;
    SkipListNode* currHead = pList->topHead;
    while (currHead != NULL)
    {
        SkipListNode* prevNode = NULL;
        SkipListNode* currNode = currHead->next;
        while (currNode != NULL)
        {
            prevNode = currNode;
            currNode = currNode->next;
            free(prevNode);
        }
        prevHead = currHead;
        currHead = currHead->down;
        free(prevHead);
    }
    free(pList);
}

SkipList* SkipList_add(SkipList* pList, const int pValue)
{
    unsigned int logSize;

    if (pList->topHead == NULL)
    {
        SkipListNode* lastNode = NULL;
        unsigned int i;
        printf("BRAND NEW!!\n");
        for (i = 0; i < pList->numLevels; i++)
        {
            SkipListNode* newNode = malloc(sizeof(SkipListNode));
            newNode->value = pValue;
            newNode->next = NULL;
            newNode->down = lastNode;
            newNode->distNext = 0;

            lastNode = newNode;
        }

        pList->topHead = lastNode;
    }
    else
    {
        SkipListNode* topHead = pList->topHead;

        printf("ADD!!\n");
        if (pValue >= topHead->value)
        {
            printf("GREATER!!\n");
            _SkipList_addNonHead(pList, pValue, TRUE);
        }
        else if (pValue < topHead->value)
        {
            printf("LESS!!\n");
            _SkipList_addHead(pList, pValue);
        }

    }

    printf("FINISHED ADD\n");

    pList->size++;

    logSize = (pList->size == 0) ? 0 : Util_log2((double)pList->size);
    if (logSize > pList->numLevels && pList->numLevels < pList->maxLevels)
    {
        printf("INCREASING (%u vs %u)\n", logSize, pList->numLevels);
        SkipList_addNewLevel(pList);
    }

    return pList;
}

SkipList* SkipList_addNewLevel(SkipList* pList)
{
    SkipListNode* currTopNode = pList->topHead;
    SkipListNode* prevNewNode;
    unsigned int i = 0;
    unsigned int currDist = 0U;
    printf("LINKING OLD HEADS\n");
    while (currTopNode != NULL)
    {
        if (i == 0 || randChance())
        {
            SkipListNode* newNode = malloc(sizeof(SkipListNode));
            newNode->value = currTopNode->value;
            newNode->next = NULL;
            newNode->distNext = 0U;
            newNode->down = currTopNode;
            if (i == 0)
            {
                pList->topHead = newNode;
            }
            else
            {
                prevNewNode->next = newNode;
                prevNewNode->distNext = currDist;
            }
            prevNewNode = newNode;
            currDist = 0U;
        }

        currDist += currTopNode->distNext;
        currTopNode = currTopNode->next;
        i++;
    }

    pList->numLevels++;

    return pList;
}

Boolean SkipList_contains(SkipList* pList, const int pValue)
{
    SkipListNode* currNode = pList->topHead;
    Boolean retVal = FALSE;

    while (1)
    {
        if (currNode->value == pValue)
        {
            retVal = TRUE;
            break;
        }
        else if (currNode->next != NULL && pValue >= currNode->next->value)
        {
            currNode = currNode->next;
        }
        else if (currNode->down != NULL)
        {
            currNode = currNode->down;
        }
        else
        {
            break;
        }
    }

    return retVal;
}