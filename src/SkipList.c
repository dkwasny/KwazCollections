#include "SkipList.h"
#include "Util.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static const unsigned int DEFAULT_MAX_LEVELS = 40U;
static const unsigned int DEFAULT_NEXT_LEVEL_CHANCE = 50U;

static int randChance(const SkipList* pList)
{
    return rand() % 100U < pList->nextLevelChance;
}

static unsigned int rollLevels(const SkipList* pList)
{
    unsigned int retVal = 1;
    unsigned int i;
    Boolean passing = TRUE;
    for (i = 1; i < pList->numLevels && passing; i++) {
        if (randChance(pList))
        {
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
    unsigned int numNewLevels = rollLevels(pList);
    unsigned int* totalDistPerLevel = malloc(sizeof(unsigned int) * pList->numLevels);
    unsigned int totalDist = 0U;
    SkipListNode* currNode = pList->topHead;
    SkipListNode* prevNewNode = NULL;
    SkipListNode** closestNodes = malloc(sizeof(SkipListNode*) * pList->numLevels);
    while (currNode != NULL)
    {
        Boolean goRight = currNode->next != NULL
            && (
                pValue > currNode->next->value
                || (dupesInBack && pValue == currNode->next->value)
            );
        if (goRight)
        {
            totalDist += currNode->distNext;
            currNode = currNode->next;
        }
        else if (currNode->down != NULL)
        {
            totalDistPerLevel[i] = totalDist;
            closestNodes[i--] = currNode;
            currNode = currNode->down;
        }
        else
        {
            totalDistPerLevel[i] = totalDist;
            closestNodes[i] = currNode;
            currNode = NULL;
        }
    }

    for (i = 0; i < pList->numLevels; i++)
    {
        if (i < numNewLevels)
        {
            unsigned int leftDistance = (totalDist - totalDistPerLevel[i]) + 1;

            SkipListNode* newNode = malloc(sizeof(SkipListNode));
            newNode->value = pValue;
            newNode->next = closestNodes[i]->next;
            newNode->down = prevNewNode;

            if (closestNodes[i]->next == NULL)
            {
                newNode->distNext = 0U;
            }
            else
            {
                unsigned int rightDistance = closestNodes[i]->distNext - (totalDist - totalDistPerLevel[i]);
                newNode->distNext = rightDistance;
            }

            closestNodes[i]->next = newNode;
            closestNodes[i]->distNext = leftDistance;

            prevNewNode = newNode;
        }
        else if (closestNodes[i]->next != NULL)
        {
            closestNodes[i]->distNext += 1;
        }
    }

    free(totalDistPerLevel);
    free(closestNodes);
}

static void _SkipList_addHead(SkipList* pList, int pValue)
{
    SkipListNode* currHead = pList->topHead;
    int oldHeadValue = currHead->value;

    while (currHead != NULL)
    {
        currHead->value = pValue;
        currHead = currHead->down;
    }

    _SkipList_addNonHead(pList, oldHeadValue, FALSE);
}

static Boolean _SkipList_removeHead(SkipList* pList)
{
    Boolean retVal = FALSE;
    SkipListNode* currNode = pList->topHead;
    if (pList->size == 1)
    {
        while (currNode != NULL)
        {
            SkipListNode* tmpNode = currNode->down;
            free(currNode);
            currNode = tmpNode;
        }
        pList->topHead = NULL;
        pList->size--;
        retVal = TRUE;
    }
    else
    {
        int bottomNextValue;
        while (currNode->down != NULL)
        {
            currNode = currNode->down;
        }
        bottomNextValue = currNode->next->value;

        currNode = pList->topHead;
        while (currNode != NULL)
        {
            if (currNode->next != NULL && currNode->next->value == bottomNextValue)
            {
                SkipListNode* nextNode = currNode->next;
                currNode->next = nextNode->next;
                currNode->distNext = (currNode->distNext + nextNode->distNext) - 1;
                free(nextNode);
            }
            else if (currNode->next != NULL)
            {
                currNode->distNext--;
            }
            currNode->value = bottomNextValue;

            currNode = currNode->down;
        }
        pList->size--;
        retVal = TRUE;
    }
    return retVal;
}

static Boolean _SkipList_removeNonHead(SkipList* pList, const int pValue)
{
    Boolean retVal = FALSE;
    SkipListNode* currNode = pList->topHead;
    unsigned int i = pList->numLevels - 1;
    SkipListNode** closestNodes = malloc(sizeof(SkipListNode*) * pList->numLevels);
    while (currNode != NULL)
    {
        if (currNode->next != NULL && pValue > currNode->next->value)
        {
            currNode = currNode->next;
        }
        else if (currNode->down != NULL)
        {
            closestNodes[i--] = currNode;
            currNode = currNode->down;
        }
        else
        {
            closestNodes[i] = currNode;
            currNode = NULL;
        }
    }

    if (closestNodes[0]->next != NULL && closestNodes[0]->next->value == pValue)
    {
        for (i = 0; i < pList->numLevels; i++)
        {
            SkipListNode* thisNode = closestNodes[i];
            SkipListNode* nextNode = thisNode->next;
            if (nextNode != NULL && nextNode->value == pValue)
            {
                thisNode->next = nextNode->next;
                thisNode->distNext = (thisNode->distNext + nextNode->distNext) - 1;
                free(nextNode);
            }
            else if (nextNode != NULL)
            {
                thisNode->distNext--;
            }
        }
        pList->size--;
        retVal = TRUE;
    }

    free(closestNodes);
    return retVal;
}

SkipList* SkipList_create(void)
{
    return SkipList_createFull(1U, DEFAULT_MAX_LEVELS, DEFAULT_NEXT_LEVEL_CHANCE);
}

SkipList* SkipList_createFull(
    const unsigned int pNumLevels,
    const unsigned int pMaxLevels,
    const unsigned int pNextLevelChance)
{
    SkipList* retVal =  malloc(sizeof(SkipList));

    unsigned int numLevels = (pNumLevels < 1U) ? 1U : pNumLevels;
    unsigned int maxLevels = (pMaxLevels < numLevels) ? numLevels : pMaxLevels;
    unsigned int nextLevelChance = (pNextLevelChance > 100U)
        ? DEFAULT_NEXT_LEVEL_CHANCE
        : pNextLevelChance;

    retVal->size = 0U;
    retVal->numLevels = numLevels;
    retVal->maxLevels = maxLevels;
    retVal->nextLevelChance = nextLevelChance;
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

        if (pValue >= topHead->value)
        {
            _SkipList_addNonHead(pList, pValue, TRUE);
        }
        else if (pValue < topHead->value)
        {
            _SkipList_addHead(pList, pValue);
        }

    }

    pList->size++;

    logSize = (pList->size == 0) ? 0 : Util_log2((double)pList->size);
    if (logSize > pList->numLevels && pList->numLevels < pList->maxLevels)
    {
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
    while (currTopNode != NULL)
    {
        if (i == 0 || randChance(pList))
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

    while (currNode != NULL)
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
            currNode = NULL;
        }
    }

    return retVal;
}

int SkipList_get(SkipList* pList, const unsigned int index)
{
    SkipListNode* currNode = pList->topHead;

    /*
        TODO Using -1 as error value because I'm lazy.
        Might add proper error handling if I'm bored in the future.
    */
    int retVal = -1;
    unsigned int currDist = 0;

    if (index >= pList->size)
    {
        return retVal;
    }

    while (currNode != NULL)
    {
        if (currDist == index)
        {
            retVal = currNode->value;
            break;
        }
        else if (currNode->next != NULL && currDist + currNode->distNext <= index)
        {
            currDist += currNode->distNext;
            currNode = currNode->next;
        }
        else if (currNode->down != NULL)
        {
            currNode = currNode->down;
        }
        else
        {
            currNode = NULL;
        }
    }

    return retVal;
}

Boolean SkipList_remove(SkipList* pList, const int pValue)
{
    Boolean retVal = FALSE;

    if (pList->size > 0)
    {
        if (pList->topHead->value == pValue)
        {
            retVal = _SkipList_removeHead(pList);
        }
        else
        {
            retVal = _SkipList_removeNonHead(pList, pValue);
        }
    }

    return retVal;
}
