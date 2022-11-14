#ifndef _C_KWAZ_SKIPLIST_
#define _C_KWAZ_SKIPLIST_

#include <stdlib.h>

typedef struct SkipList SkipList;
typedef struct SkipListNode SkipListNode;
typedef struct SkipListEdge SkipListEdge;

struct SkipList
{
    size_t size;
    size_t numLevels;
    size_t maxLevels;
    SkipListNode* heads;
};

struct SkipListNode
{
    int value;
    SkipListNode* next;
    SkipListNode* down;
    /* TODO node distances someday */
    /*size_t distNext;*/
};

#ifdef __cplusplus
extern "C" {
#endif

SkipList* SkipList_create(void);

void SkipList_destroy(SkipList* pList);

SkipList* SkipList_add(SkipList* pList, const int pValue);

/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
