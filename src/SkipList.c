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

static void _SkipList_addNonHead(SkipList* pList, int pValue, Boolean dupesInBack)
{
    size_t i = pList->numLevels - 1;
    size_t numNewLevels = rollLevels(pList->numLevels);
    SkipListNode* currNode = pList->topHead;
    SkipListNode* prevNewNode = NULL;
    SkipListNode** closestNodes = malloc(sizeof(SkipListNode*) * pList->numLevels);
    printf("KWAS TOPHEAD HAS DOWN? - %d\n", pList->topHead->down != NULL);
    while(1)
    {
        Boolean goRight = currNode->next != NULL
            && (
                pValue > currNode->next->value
                || (dupesInBack && pValue == currNode->next->value)
            );
        if (goRight)
        {
            printf("RIGHT\n");
            currNode = currNode->next;
        }
        else if (currNode->down != NULL)
        {
            printf("DOWN - %lu\n", i);
            closestNodes[i--] = currNode;
            currNode = currNode->down;
        }
        else
        {
            printf("BREAK - %lu\n", i);
            closestNodes[i] = currNode;
            break;
        }
    }

    printf("ADD - NEW LEVELS (%lu)\n", numNewLevels);
    for (i = 0; i < numNewLevels; i++)
    {
        SkipListNode* newNode = malloc(sizeof(SkipListNode));
        newNode->value = pValue;
        newNode->next = closestNodes[i]->next;
        newNode->down = prevNewNode;

        closestNodes[i]->next = newNode;

        prevNewNode = newNode;
    }

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
    SkipList* retVal =  malloc(sizeof(SkipList));

    retVal->size = 0U;
    retVal->numLevels = 1U;
    retVal->maxLevels = 5U;
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
    size_t logSize;

    if (pList->topHead == NULL)
    {
        SkipListNode* newNode = malloc(sizeof(SkipListNode));
        printf("BRAND NEW!!\n");
        newNode->value = pValue;
        newNode->next = NULL;
        newNode->down = NULL;
        pList->topHead = newNode;
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
        printf("INCREASING (%lu vs %lu)\n", logSize, pList->numLevels);
        SkipList_addNewLevel(pList);
    }

    return pList;
}

SkipList* SkipList_addNewLevel(SkipList* pList)
{
    SkipListNode* currTopNode = pList->topHead;
    if (currTopNode != NULL)
    {
        SkipListNode* prevNewNode;
        SkipListNode* newTopNode;
        newTopNode = malloc(sizeof(SkipListNode));
        newTopNode->value = currTopNode->value;
        newTopNode->next = NULL;
        newTopNode->down = currTopNode;

        pList->topHead = newTopNode;

        prevNewNode = newTopNode;
        currTopNode = currTopNode->next;
        printf("LINKING OLD HEADS\n");
        while (currTopNode != NULL)
        {
            if (randChance())
            {
                SkipListNode* newNode = malloc(sizeof(SkipListNode));
                newNode->value = currTopNode->value;
                newNode->next = NULL;
                newNode->down = currTopNode;
                prevNewNode->next = newNode;
                prevNewNode = newNode;
            }
            currTopNode = currTopNode->next;
        }

        pList->numLevels++;
    }

    return pList;
}
