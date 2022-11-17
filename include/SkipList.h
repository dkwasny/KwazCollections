#ifndef _C_KWAZ_SKIPLIST_
#define _C_KWAZ_SKIPLIST_

#include "Boolean.h"
#include <stdlib.h>

typedef struct SkipList SkipList;
typedef struct SkipListNode SkipListNode;

struct SkipList
{
    unsigned int size;
    unsigned int numLevels;
    unsigned int maxLevels;
    unsigned int nextLevelChance;
    SkipListNode* topHead;
};

struct SkipListNode
{
    int value;
    SkipListNode* next;
    SkipListNode* down;
    unsigned int distNext;
};

#ifdef __cplusplus
extern "C" {
#endif

SkipList* SkipList_create(void);
SkipList* SkipList_createFull(
    const unsigned int pNumLevels,
    const unsigned int pMaxLevels,
    const unsigned int pNextLevelChance
);

void SkipList_destroy(SkipList* pList);
SkipList* SkipList_add(SkipList* pList, const int pValue);
SkipList* SkipList_addNewLevel(SkipList* pList);
Boolean SkipList_contains(SkipList* pList, const int pValue);
int SkipList_get(SkipList* pList, const unsigned int index);

/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
