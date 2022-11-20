#include "gtest/gtest.h"
#include "BTree.h"
#include "ArrayList.h"
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

static void visitNode(BTreeNode* pNode, ArrayList* pVisited)
{
    int i = 0;
    for (; i < pNode->numValues; i++)
    {
        if (pNode->pointers != NULL)
        {
            visitNode(pNode->pointers[i], pVisited);
        }
        ArrayList_add(pVisited, pNode->values[i]);
    }

    if (pNode->pointers != NULL)
    {
        visitNode(pNode->pointers[i], pVisited);
    }
}

static void validateTree(BTree* pTree, unsigned int expectedSize)
{
    ArrayList* visited = ArrayList_create();
    visitNode(pTree->root, visited);

    ASSERT_EQ(expectedSize, visited->size);
    ASSERT_EQ(expectedSize, pTree->size);
    for (int i = 0; i < pTree->size; i++)
    {
        ASSERT_EQ(i + 1, ArrayList_get(visited, i));
    }

    ArrayList_destroy(visited);
}

TEST(BTree, BackEndInsert)
{
    BTree* tree = BTree_create();

    for (int i = 1; i <= 10000; i++)
    {
        BTree_add(tree, i);
    }

    validateTree(tree, 10000);

    BTree_destroy(tree);
}

TEST(BTree, FrontEndInsert)
{
    BTree* tree = BTree_create();

    for (int i = 10000; i > 0; i--)
    {
        BTree_add(tree, i);
    }

    validateTree(tree, 10000);

    BTree_destroy(tree);
}

TEST(BTree, MiddleInsert)
{
    BTree* tree = BTree_create();

    int x = 3000;
    int y = 6000;

    for (int i = 1; i < x; i++)
    {
        BTree_add(tree, i);
    }
    for (int i = y; i <= 10000; i++)
    {
        BTree_add(tree, i);
    }
    for (int i = x; i < y; i++)
    {
        BTree_add(tree, i);
    }

    validateTree(tree, 10000);

    BTree_destroy(tree);
}

TEST(BTree, ZipperInsert)
{
    BTree* tree = BTree_create();

    for (int i = 1; i <= 10000; i+=2)
    {
        BTree_add(tree, i);
    }
    for (int i = 2; i <= 10000; i+=2)
    {
        BTree_add(tree, i);
    }

    validateTree(tree, 10000);

    BTree_destroy(tree);
}