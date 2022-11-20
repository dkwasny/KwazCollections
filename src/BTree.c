#include "BTree.h"
#include "Boolean.h"
#include <stdlib.h>
#include <stdio.h>

static void _BTreeNode_shallowDestroy(BTreeNode* pNode)
{
    if (pNode->pointers != NULL)
    {
        free(pNode->pointers);
    }
    free(pNode->values);
    free(pNode);
}

static void _BTreeNode_destroy(BTreeNode* pNode)
{
    if (pNode->pointers != NULL)
    {
        unsigned int i = 0;
        for (; i <= pNode->numValues; i++)
        {
            _BTreeNode_destroy(pNode->pointers[i]);
        }
    }
    _BTreeNode_shallowDestroy(pNode);
}

static BTreeNode* _BTreeNode_addValue(BTreeNode* pNode, BTree* pTree, const int pValue)
{
    BTreeNode* retVal = NULL;
    BTreeNode* newNode = NULL;
    unsigned int i;
    int valueToAdd = pValue;
    Boolean fullNode = pNode->numValues >= pTree->order - 1;
    Boolean leafNode = pNode->pointers == NULL;
    for (i = 0; i < pNode->numValues; i++)
    {
        if (pNode->values[i] >= valueToAdd)
        {
            break;
        }
    }

    if (!leafNode)
    {
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
                _BTreeNode_shallowDestroy(pNode->pointers[i]);
                pNode->pointers[i] = newNode->pointers[0];
                pNode->pointers[i + 1] = newNode->pointers[1];
            }

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
            i = 0;
            while (i < pNode->numValues)
            {
                int newVal = pNode->values[i];
                if (i == j && valueToAdd < newVal)
                {
                    newVal = valueToAdd;
                    if (!leafNode)
                    {
                        tmpPtrArray[k++] = newNode->pointers[0];
                        tmpPtrArray[k++] = newNode->pointers[1];
                        _BTreeNode_shallowDestroy(pNode->pointers[l]);
                        l++;
                    }
                }
                else
                {
                    if (!leafNode)
                    {
                        tmpPtrArray[k++] = pNode->pointers[l++];
                    }
                    i++;
                }
                tmpArray[j++] = newVal;
            }

            if (i == j)
            {
                tmpArray[j] = valueToAdd;
                if (!leafNode)
                {
                    _BTreeNode_shallowDestroy(pNode->pointers[l]);
                    tmpPtrArray[k++] = newNode->pointers[0];
                    tmpPtrArray[k] = newNode->pointers[1];
                }
            }

            medianIndex = tmpArraySize / 2;

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
            for (i = medianIndex + 1; i < tmpArraySize; i++)
            {
                newRight->values[j] = tmpArray[i];
                newRight->numValues++;
                j++;
            }

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

                for (i = 0; i < tmpPtrArraySize; i++)
                {
                    if (i < medianIndex)
                    {
                        newLeft->pointers[myI++] = tmpPtrArray[i];
                    }
                    else if (i == medianIndex)
                    {
                        newLeft->pointers[myI] = tmpPtrArray[i++];
                        myI = 0;
                        newRight->pointers[myI++] = tmpPtrArray[i];
                    }
                    else
                    {
                        newRight->pointers[myI++] = tmpPtrArray[i];
                    }
                }
            }

            free(tmpArray);
            free(tmpPtrArray);
        }
    }

    if (newNode != NULL)
    {
        _BTreeNode_shallowDestroy(newNode);
    }

    return retVal;
}

BTree* BTree_create()
{
    BTree* retVal = malloc(sizeof(BTree));

    retVal->size = 0U;
    retVal->order = 3U;
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
            _BTreeNode_shallowDestroy(pTree->root);
            pTree->root = newRoot;
        }
    }

    pTree->size++;

    return pTree;
}

void BTree_destroy(BTree* pTree)
{
    if (pTree != NULL)
    {
        _BTreeNode_destroy(pTree->root);
        free(pTree);
    }
}