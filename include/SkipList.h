#ifndef _C_KWAZ_SKIPLIST_
#define _C_KWAZ_SKIPLIST_

#include "Boolean.h"
#include <stdlib.h>

typedef struct SkipList SkipList;
typedef struct SkipListNode SkipListNode;
typedef struct SkipListEdge SkipListEdge;

struct SkipList
{
    size_t size;
    size_t numLevels;
    size_t maxLevels;
    SkipListNode* topHead;
};

struct SkipListNode
{
    int value;
    SkipListNode* next;
    SkipListNode* down;
    size_t distNext;
};

#ifdef __cplusplus
extern "C" {
#endif

SkipList* SkipList_create(void);
SkipList* SkipList_createFull(const size_t pNumLevels, const size_t pMaxLevels);

void SkipList_destroy(SkipList* pList);

SkipList* SkipList_add(SkipList* pList, const int pValue);

SkipList* SkipList_addNewLevel(SkipList* pList);

Boolean SkipList_contains(SkipList* pList, const int pValue);

/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
