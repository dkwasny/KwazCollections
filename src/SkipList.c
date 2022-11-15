#include "SkipList.h"
#include "Util.h"
#include "Boolean.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

static int randChance(void)
{
    return rand() % 100 < 50;
}

static size_t rollLevels(size_t numLevels)
{
    size_t retVal = 1;
    size_t i;
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

static SkipListNode* _SkipList_getTopHead(SkipList* pList)
{
    return pList->heads + (pList->numLevels - 1);
}

static void _SkipList_addNonHead(SkipList* pList, int pValue)
{
    size_t i;
    size_t numNewLevels = rollLevels(pList->numLevels);
    SkipListNode* prevNewBlah;
    SkipListNode* currNode = _SkipList_getTopHead(pList);
    SkipListNode* newNode = malloc(sizeof(SkipListNode));
    newNode->value = pValue;
    newNode->next = NULL;
    newNode->down = NULL;
    printf("GREATER!!\n");
    while(1)
    {
        Boolean goRight = currNode->next != NULL
            && pValue >= currNode->next->value;
        if (goRight)
        {
            printf("RIGHT\n");
            currNode = currNode->next;
        }
        else if (currNode->down != NULL)
        {
            printf("DOWN\n");
            currNode = currNode->down;
        }
        else
        {
            printf("BREAK\n");
            break;
        }
    }

    if (currNode->next == NULL)
    {
        currNode->next = newNode;
    }
    else
    {
        newNode->next = currNode->next;
        currNode->next = newNode;
    }

    prevNewBlah = newNode;
    printf("ADD - NEW LEVELS (%lu)\n", numNewLevels);
    for (i = 1; i < numNewLevels; i++)
    {
        SkipListNode* currHead = pList->heads + i;
        SkipListNode* newBlah;
        while (currHead->next != NULL && currHead->next->value < pValue)
        {
            currHead = currHead->next;
        }
        newBlah = malloc(sizeof(SkipListNode));
        newBlah->value = pValue;
        newBlah->next = currHead->next;
        newBlah->down = prevNewBlah;

        currHead->next = newBlah;

        prevNewBlah = newBlah;
    }
}

static void _SkipList_addHead(SkipList* pList, int pValue)
{
    size_t i;
    SkipListNode* prevOldHead = NULL;
    size_t newLevelCount = rollLevels(pList->numLevels);

    SkipListNode* newHeads = malloc(sizeof(SkipListNode) * pList->numLevels);

    SkipListNode* currFirstLevelHead = pList->heads;
    SkipListNode* firstLevelOldHead = malloc(sizeof(SkipListNode));
    printf("ALLOC 1\n");
    firstLevelOldHead->value = currFirstLevelHead->value;
    firstLevelOldHead->next = currFirstLevelHead->next;
    firstLevelOldHead->down = NULL;

    printf("LESS!!\n");

    newHeads[0].value = pValue;
    newHeads[0].next = firstLevelOldHead;
    newHeads[0].down = NULL;

    prevOldHead = firstLevelOldHead;
    for (i = 1; i < pList->numLevels; i++)
    {
        if (i < newLevelCount)
        {
            SkipListNode* oldHead = pList->heads + i;

            SkipListNode* newOldHead = malloc(sizeof(SkipListNode));
            printf("ALLOC 2 \n");
            newOldHead->value = oldHead->value;
            newOldHead->next = oldHead->next;
            newOldHead->down = prevOldHead;

            newHeads[i].next = newOldHead;

            prevOldHead = newOldHead;
        }
        else
        {
            newHeads[i].next = pList->heads[i].next;
        }

        newHeads[i].value = pValue;
        newHeads[i].down = (i > 0) ? newHeads + i - 1 : NULL;
    }

    free(pList->heads);
    pList->heads = newHeads;
}

SkipList* SkipList_create(void)
{
    SkipList* retVal =  malloc(sizeof(SkipList));

    retVal->size = 0U;
    retVal->numLevels = 1U;
    retVal->maxLevels = 5U;
    retVal->heads = NULL;

    return retVal;
}

void SkipList_destroy(SkipList* pList)
{
    size_t i;
    SkipListNode* currNode;
    SkipListNode* nextNode;
    for (i = 0; i < pList->numLevels; i++)
    {
        currNode = pList->heads[i].next;
        while (currNode != NULL)
        {
            nextNode = currNode->next;
            free(currNode);
            currNode = nextNode;
        }

    }
    free(pList->heads);
    free(pList);
}

SkipList* SkipList_add(SkipList* pList, const int pValue)
{
    size_t logSize;

    if (pList->heads == NULL)
    {
        SkipListNode* newNode = malloc(sizeof(SkipListNode));
        newNode->value = pValue;
        newNode->next = NULL;
        newNode->down = NULL;
        pList->heads = newNode;
    }
    else
    {
        SkipListNode* topHead = _SkipList_getTopHead(pList);

        printf("ADD!!\n");
        if (pValue >= topHead->value)
        {
            _SkipList_addNonHead(pList, pValue);
        }
        else if (pValue < topHead->value)
        {
            _SkipList_addHead(pList, pValue);
        }

    }

    printf("FINISHED ADD\n");

    pList->size++;

    logSize = (pList->size == 0) ? 0 : Util_log2((double)pList->size);
    if (logSize > pList->numLevels && pList->numLevels < pList->maxLevels)
    {
        printf("INCREASING (%lu vs %lu)\n", logSize, pList->numLevels);
        SkipList_addNewLevel(pList);
    }

    return pList;
}

SkipList* SkipList_addNewLevel(SkipList* pList)
{
    SkipListNode* currNode;
    SkipListNode* prevNewNode;
    size_t i;
    SkipListNode* topHead = pList->heads + (pList->numLevels - 1);

    SkipListNode* newHeads = malloc(sizeof(SkipListNode) * (pList->numLevels + 1));
    for (i = 0; i < pList->numLevels; i++)
    {
        newHeads[i].value = pList->heads[i].value;
        newHeads[i].next = pList->heads[i].next;
        newHeads[i].down = (i > 0) ? newHeads + i - 1 : NULL;
    }

    newHeads[pList->numLevels].value = pList->heads->value;
    newHeads[pList->numLevels].next = NULL;
    newHeads[pList->numLevels].down = newHeads + (pList->numLevels - 1);

    prevNewNode = newHeads + pList->numLevels;

    currNode = topHead->next;
    printf("LINKING OLD HEADS\n");
    while (currNode != NULL)
    {
        if (randChance())
        {
            SkipListNode* newNode = malloc(sizeof(SkipListNode));
            newNode->value = currNode->value;
            newNode->next = NULL;
            newNode->down = currNode;
            prevNewNode->next = newNode;
            prevNewNode = newNode;
        }
        currNode = currNode->next;
    }

    free(pList->heads);
    pList->heads = newHeads;
    pList->numLevels++;

    printf("FINISH INCREASING\n");

    return pList;
}
