#ifndef _C_KWAZ_ILIST_
#define _C_KWAZ_ILIST_

#include "ICollection.h"

typedef struct IList IList;

struct IList
{
	/* ICollection Methods */
	int (* const getSize)(const IList* pList);
	void (* const destroy)(IList* pCollection);
	void (* const add)(IList* pList, const int pValue);
	IIterator* (* const iterator)(IList* pList);

	/* IList Methods */
	int (* const get)(const IList* pList, const size_t pIndex);
	int (* const remove)(IList* pList, const size_t pIndex);
	
	/* Internal References */
	ICollection* superType;
	void* subType;
};

#endif
