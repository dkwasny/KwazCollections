#ifndef _C_KWAZ_BTREE_
#define _C_KWAZ_BTREE_

#include "Boolean.h"

typedef struct BTree BTree;
typedef struct BTreeNode BTreeNode;

struct BTree
{
    unsigned int size;
    unsigned int order;
    BTreeNode* root;
};

struct BTreeNode
{
    int* values;
    BTreeNode** pointers;
    unsigned int numValues;
};

#ifdef __cplusplus
extern "C" {
#endif

BTree* BTree_create(void);
BTree* BTree_createFull(const unsigned int pOrder);

BTree* BTree_add(BTree* pTree, const int pValue);

Boolean BTree_contains(BTree* pTree, const int pValue);

void BTree_destroy(BTree* pTree);

/* TODO Delete one day when I'm feeling saucy */

#ifdef __cplusplus
}
#endif

#endif
