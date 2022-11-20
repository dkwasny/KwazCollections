#include "BTree.h"
#include "Boolean.h"
#include <stdlib.h>
#include <stdio.h>

static BTreeNode* _BTreeNode_addValue(BTreeNode* pNode, BTree* pTree, const int pValue)
{
    BTreeNode* retVal = NULL;
    BTreeNode* newNode = NULL;
    unsigned int i;
    int valueToAdd = pValue;
    Boolean fullNode = pNode->numValues >= pTree->order - 1;
    Boolean leafNode = pNode->pointers == NULL;
    printf("----- START ADD - %d\n", valueToAdd);
    for (i = 0; i < pNode->numValues; i++)
    {
        if (pNode->values[i] >= valueToAdd)
        {
            break;
        }
    }
    printf("I = %d\n", i);

    /* recurse down to bottom */
    /* TODO I think we can always assume at least one value*/
    if (!leafNode)
    {
        printf("RECURSING\n");
        newNode = _BTreeNode_addValue(pNode->pointers[i], pTree, valueToAdd);
        if (newNode != NULL)
        {
            valueToAdd = newNode->values[0];
        }
    }
    
    if (leafNode || newNode != NULL)
    {
        if (!fullNode)
        {
            unsigned int j;
            printf("EASY ADD\n");
            for (j = pNode->numValues; j > i; j--)
            {
                pNode->values[j] = pNode->values[j - 1];
            }
            pNode->values[i] = valueToAdd;

            if (newNode != NULL)
            {
                for (j = pNode->numValues + 1; j > i + 1; j--)
                {
                    pNode->pointers[j] = pNode->pointers[j - 1];
                }
                pNode->pointers[i] = newNode->pointers[0];
                pNode->pointers[i + 1] = newNode->pointers[1];
            }

            /* TODO WHAT ABOUT pointer[i] ?!?!?*/
            pNode->numValues++;
        }
        else
        {
            BTreeNode* newLeft;
            BTreeNode* newRight;
            unsigned int medianIndex;
            unsigned int j = 0;
            unsigned int k = 0;
            unsigned int l = 0;
            const unsigned int tmpArraySize = pNode->numValues + 1;
            const unsigned int tmpPtrArraySize = tmpArraySize + 1;
            int* tmpArray = malloc(sizeof(int) * tmpArraySize);
            BTreeNode** tmpPtrArray = malloc(sizeof(BTreeNode*) * tmpPtrArraySize);
            printf("SPLITTING\n");
            i = 0;
            while (i < pNode->numValues)
            {
                int newVal = pNode->values[i];
                if (i == j && valueToAdd < newVal)
                {
                    printf("SPECIAL SAUCE\n");
                    newVal = valueToAdd;
                    if (!leafNode)
                    {
                        printf("SPECIAL SAUCE NON LEAF\n");
                        tmpPtrArray[k++] = newNode->pointers[0];
                        tmpPtrArray[k++] = newNode->pointers[1];
                        l++;
                    }
                }
                else
                {
                    if (!leafNode)
                    {
                        printf("KWAS I = %d\n", i);
                        printf("P->P[%d] is %d\n", i, (pNode->pointers[i] == NULL) ? 1 : 0);
                        tmpPtrArray[k++] = pNode->pointers[l++];
                    }
                    i++;
                }
                tmpArray[j++] = newVal;
                printf("TMPARRAY I=%u J=%u\n", i, j);
            }

            if (i == j)
            {
                tmpArray[j] = valueToAdd;
                if (!leafNode)
                {
                    printf("ADDING PTR FOR NEW NODE ON END FOR %d and %d\n", k, k+1);
                    tmpPtrArray[k++] = newNode->pointers[0];
                    tmpPtrArray[k] = newNode->pointers[1];
                }
            }

            printf("J = %d - K = %d\n", j, k);

            medianIndex = tmpArraySize / 2;

            printf("CREATING LEFT\n");
            /* Create left node */
            newLeft = malloc(sizeof(BTreeNode));
            newLeft->values = malloc(sizeof(int) * (pTree->order - 1));
            newLeft->pointers = NULL;
            if (!leafNode)
            {
                newLeft->pointers = malloc(sizeof(BTreeNode*) * pTree->order);
            }
            newLeft->numValues = 0U;
            for (i = 0; i < medianIndex; i++)
            {
                newLeft->values[i] = tmpArray[i];
                newLeft->numValues++;
            }

            printf("CREATING RIGHT\n");
            /* Create right node */
            newRight = malloc(sizeof(BTreeNode));
            newRight->values = malloc(sizeof(int) * (pTree->order - 1));
            newRight->pointers = NULL;
            if (!leafNode)
            {
                newRight->pointers = malloc(sizeof(BTreeNode*) * pTree->order);
            }
            newRight->numValues = 0U;
            j = 0;
            printf("I IS %u AND SIZE IS %u\n", medianIndex + 1, tmpArraySize);
            for (i = medianIndex + 1; i < tmpArraySize; i++)
            {
                printf("COPYING TO RIGHT - %d\n", tmpArray[i]);
                newRight->values[j] = tmpArray[i];
                newRight->numValues++;
                j++;
            }

            printf("CREATING RETVAL\n");
            /* create dummy node TODO keep this dummy??? */
            retVal = malloc(sizeof(BTreeNode));
            retVal->values = malloc(sizeof(int) * (pTree->order - 1));
            retVal->values[0] = tmpArray[medianIndex];
            retVal->pointers = malloc(sizeof(BTreeNode*) * pTree->order);
            retVal->pointers[0] = newLeft;
            retVal->pointers[1] = newRight;
            retVal->numValues = 1;

            if (retVal != NULL && !leafNode)
            {
                unsigned int myI = 0;

                printf("MEDIAN INDEX = %d\n", medianIndex);
                for (i = 0; i < tmpPtrArraySize; i++)
                {
                    if (i < medianIndex)
                    {
                        printf("LEFT %d == TMP %d\n", myI, i);
                        newLeft->pointers[myI++] = tmpPtrArray[i];
                    }
                    else if (i == medianIndex)
                    {
                        printf("LEFT %d == TMP %d\n", myI, i);
                        newLeft->pointers[myI] = tmpPtrArray[i++];
                        myI = 0;
                        printf("RIGHT %d == TMP %d\n", myI, i);
                        newRight->pointers[myI++] = tmpPtrArray[i];
                    }
                    else
                    {
                        printf("RIGHT %d == TMP %d\n", myI, i);
                        newRight->pointers[myI++] = tmpPtrArray[i];
                    }
                }
            }

            free(tmpArray);
        }
    }

    printf("END ADD\n");

    return retVal;
}

BTree* BTree_create()
{
    BTree* retVal = malloc(sizeof(BTree));

    retVal->size = 0U;
    retVal->order = 1U;
    retVal->root = NULL;

    return retVal;
}

BTree* BTree_add(BTree* pTree, const int pValue)
{
    if (pTree->size == 0)
    {
        BTreeNode* newRoot = malloc(sizeof(BTreeNode));
        newRoot->values = malloc(sizeof(int) * (pTree->order - 1));
        newRoot->pointers = NULL;
        newRoot->numValues = 1U;

        newRoot->values[0] = pValue;

        pTree->root = newRoot;
    }
    else
    {
        BTreeNode* newRoot = _BTreeNode_addValue(pTree->root, pTree, pValue);
        if (newRoot != NULL)
        {
            pTree->root = newRoot;
        }
    }

    pTree->size++;

    return pTree;
}

void BTree_destroy(BTree* pTree)
{
    /* TODO Traverse and free the world */
    free(pTree);
}