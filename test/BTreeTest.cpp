#include "gtest/gtest.h"
#include "BTree.h"
#include <stdio.h>

static void printNode(BTreeNode* pNode, int depth)
{
    printf(" PNODE IS NULL: %d\n", pNode == NULL);
    printf("---------- depth: %d - numValues: %u\n[", depth, pNode->numValues);
    for (int i = 0; i < pNode->numValues - 1; i++)
    {
        printf("%d, ", pNode->values[i]);
    }
    printf("%d] ", pNode->values[pNode->numValues - 1]);

    if (pNode->pointers == NULL)
    {
        printf("Leaf Node\n");
    }
    else
    {
        printf("\n");
        for (int i = 0; i <= pNode->numValues; i++)
        {
            printNode(pNode->pointers[i], depth + 1);
        }
    }
}

static void printTree(BTree* pTree)
{
    printf("Size: %u - Order: %u\n", pTree->size, pTree->order);
    printNode(pTree->root, 0);
}


TEST(BTree, BackEndAdd)
{
    BTree* tree = BTree_create();

    for (int i = 1; i < 9; i++)
    {
        BTree_add(tree, i);
        // printTree(tree);
    }

    printTree(tree);

    BTree_destroy(tree);
}

TEST(BTree, FrontEndAdd)
{
    BTree* tree = BTree_create();

    for (int i = 8; i > 0; i--)
    {
        BTree_add(tree, i);
        // printTree(tree);
    }

    printTree(tree);

    BTree_destroy(tree);
}

TEST(BTree, MiddleAdd)
{
    BTree* tree = BTree_create();

    int values[7] = {1, 2, 20, 50, 51, 22, 23};

    for (int i = 0; i < 7; i++)
    {
        BTree_add(tree, values[i]);
    }

    printTree(tree);

    BTree_destroy(tree);
}