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

static BTreeNode* _BTreeNode_split(BTreeNode* pNode, int valueToAdd, BTreeNode* dummyNode, BTree* pTree)
{
    BTreeNode* retVal = NULL;
    const Boolean leafNode = pNode->pointers == NULL;
    BTreeNode* newLeft;
    BTreeNode* newRight;
    unsigned int medianIndex = 0;
    unsigned int tmpValueIdx = 0;
    unsigned int tmpPtrIdx = 0;
    unsigned int ptrIdx = 0;
    unsigned int valueIdx = 0;
    unsigned int rightValueIdx = 0;
    const unsigned int tmpArraySize = pNode->numValues + 1;
    const unsigned int tmpPtrArraySize = tmpArraySize + 1;
    int* tmpArray = malloc(sizeof(int) * tmpArraySize);
    BTreeNode** tmpPtrArray = malloc(sizeof(BTreeNode*) * tmpPtrArraySize);

    /*
        Gather the values and pointers from both the current node
        and the potentially new dummy node into temporary arrays
        so they can be easily divvied out to this node's new
        left and right nodes.
    */
    while (valueIdx < pNode->numValues)
    {
        int newVal = pNode->values[valueIdx];
        /*
            `tmpValueIdx` will diverge from `valueIdx` once I handle
            gathering the pointers and values from the new dummy
            node.
            I use this equality check to ensure we don't add the
            dummy node data multiple times.
        */
        if (valueIdx == tmpValueIdx && valueToAdd < newVal)
        {
            newVal = valueToAdd;
            if (!leafNode)
            {
                tmpPtrArray[tmpPtrIdx++] = dummyNode->pointers[0];
                tmpPtrArray[tmpPtrIdx++] = dummyNode->pointers[1];
                _BTreeNode_shallowDestroy(pNode->pointers[ptrIdx++]);
            }
        }
        else
        {
            if (!leafNode)
            {
                tmpPtrArray[tmpPtrIdx++] = pNode->pointers[ptrIdx++];
            }
            valueIdx++;
        }
        tmpArray[tmpValueIdx++] = newVal;
    }

    /*
        If `valueIdx` and `tmpValueIdx` still haven't diverged, then
        we can assume that the dummy node's values and pointers are
        the last ones to be gathered.
    */
    if (valueIdx == tmpValueIdx)
    {
        tmpArray[tmpValueIdx] = valueToAdd;
        if (!leafNode)
        {
            _BTreeNode_shallowDestroy(pNode->pointers[ptrIdx]);
            tmpPtrArray[tmpPtrIdx++] = dummyNode->pointers[0];
            tmpPtrArray[tmpPtrIdx] = dummyNode->pointers[1];
        }
    }

    /*
        Pick and index to split the temp arrays on and perform
        the split.
    */
    medianIndex = tmpArraySize / 2;

    /*
        Create the new left node.
        This node will contain all values and pointers that are
        smaller than the median value of this split.
    */
    newLeft = malloc(sizeof(BTreeNode));
    newLeft->values = malloc(sizeof(int) * (pTree->order - 1));
    newLeft->pointers = NULL;
    if (!leafNode)
    {
        newLeft->pointers = malloc(sizeof(BTreeNode*) * pTree->order);
        for (tmpPtrIdx = 0; tmpPtrIdx <= medianIndex; tmpPtrIdx++)
        {
            newLeft->pointers[tmpPtrIdx] = tmpPtrArray[tmpPtrIdx];
        }
    }
    newLeft->numValues = 0U;
    for (tmpValueIdx = 0; tmpValueIdx < medianIndex; tmpValueIdx++)
    {
        newLeft->values[tmpValueIdx] = tmpArray[tmpValueIdx];
        newLeft->numValues++;
    }

    /*
        Create the new right node.
        This node will contain all values and pointers that are
        larger than the median value of this split.
    */
    newRight = malloc(sizeof(BTreeNode));
    newRight->values = malloc(sizeof(int) * (pTree->order - 1));
    newRight->pointers = NULL;
    if (!leafNode)
    {
        unsigned int newPtrIdx = 0;
        newRight->pointers = malloc(sizeof(BTreeNode*) * pTree->order);
        for (tmpPtrIdx = medianIndex + 1; tmpPtrIdx < tmpPtrArraySize; tmpPtrIdx++)
        {
            newRight->pointers[newPtrIdx] = tmpPtrArray[tmpPtrIdx];
            newPtrIdx++;
        }
    }
    newRight->numValues = 0U;
    for (tmpValueIdx = medianIndex + 1; tmpValueIdx < tmpArraySize; tmpValueIdx++)
    {
        newRight->values[rightValueIdx++] = tmpArray[tmpValueIdx];
        newRight->numValues++;
    }

    /*
        Create the dummy node.
        This node isn't a "real" node, but instead a reuse of the
        BTreeNode struct to contain the split median value alongside
        the left and right child nodes, all of which are needed for
        the upstream merge.
    */
    retVal = malloc(sizeof(BTreeNode));
    retVal->values = malloc(sizeof(int) * (pTree->order - 1));
    retVal->values[0] = tmpArray[medianIndex];
    retVal->pointers = malloc(sizeof(BTreeNode*) * pTree->order);
    retVal->pointers[0] = newLeft;
    retVal->pointers[1] = newRight;
    retVal->numValues = 1;

    free(tmpArray);
    free(tmpPtrArray);

    return retVal;
}

static void _BTreeNode_merge(BTreeNode* pNode, int valueToAdd, BTreeNode* dummyNode, unsigned int valueIdx)
{
    /*
        Move all values to the right of `valueIdx` to the right by one
        spot.
    */
    unsigned int j;
    for (j = pNode->numValues; j > valueIdx; j--)
    {
        pNode->values[j] = pNode->values[j - 1];
    }
    pNode->values[valueIdx] = valueToAdd;

    /*
        Similar to the values, move all of the pointers to the right
        of `valueIdx + 1` to the right by one spot.
        We don't care about the pointer at `valueIdx` because both
        it and index `valueIdx + 1` will be replaced with the pointers
        returned from the dummy node.
    */
    if (dummyNode != NULL)
    {
        for (j = pNode->numValues + 1; j > valueIdx + 1; j--)
        {
            pNode->pointers[j] = pNode->pointers[j - 1];
        }
        _BTreeNode_shallowDestroy(pNode->pointers[valueIdx]);
        pNode->pointers[valueIdx] = dummyNode->pointers[0];
        pNode->pointers[valueIdx + 1] = dummyNode->pointers[1];
    }

    pNode->numValues++;
}

static BTreeNode* _BTreeNode_addValue(BTreeNode* pNode, BTree* pTree, const int pValue)
{
    BTreeNode* retVal = NULL;
    BTreeNode* dummyNode = NULL;
    unsigned int valueIdx;
    int valueToAdd = pValue;
    const Boolean fullNode = pNode->numValues >= pTree->order - 1;
    const Boolean leafNode = pNode->pointers == NULL;

    /*
        Find the index where the input value would fit if necessary.
    */
    for (valueIdx = 0; valueIdx < pNode->numValues; valueIdx++)
    {
        if (pNode->values[valueIdx] >= valueToAdd)
        {
            break;
        }
    }

    /*
        If there are children nodes, send the input value to the one
        corresponding to valueIdx.
    */
    if (!leafNode)
    {
        /*
            AddValue may return a "dummy node" containing a single value and
            two pointers that need to be merged into the current node.
            This merge may also cause a new split to be pushed upstream.
        */
        dummyNode = _BTreeNode_addValue(pNode->pointers[valueIdx], pTree, valueToAdd);
        if (dummyNode != NULL)
        {
            /*
                If a child addition resulted in a new value, we no longer
                care about pValue value since it has been added downstream.
                I now need to take care of the value returned from the
                downstream split.
            */
            valueToAdd = dummyNode->values[0];
        }
    }

    /*
        Values and pointers should only be edited when visiting a leaf node, or
        a dummy node was returned from a downstream split.
    */
    if (leafNode || dummyNode != NULL)
    {
        if (fullNode)
        {
            retVal = _BTreeNode_split(pNode, valueToAdd, dummyNode, pTree);
        }
        else
        {
            _BTreeNode_merge(pNode, valueToAdd, dummyNode, valueIdx);
        }

    }

    /*
        Destroy the downstream dummy node since we have merged it into this node.
    */
    if (dummyNode != NULL)
    {
        _BTreeNode_shallowDestroy(dummyNode);
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