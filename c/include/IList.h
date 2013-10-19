#ifndef _C_KWAZ_ILIST_
#define _C_KWAZ_ILIST_

#include "ICollection.h"

typedef struct IList IList;

struct IList
{
	/* ICollection Methods */
	size_t (* const getSize)(const IList* pList);
	void (* const destroy)(IList* pList);
	void (* const add)(IList* pList, const void* pValue);
	IIterator* (* const iterator)(IList* pList);

	/* IList Methods */
	void* (* const get)(const IList* pList, const size_t pIndex);
	void (* const remove)(IList* pList, const size_t pIndex);
	
	/* Internal References */
	ICollection* superType;
	void* subType;
};

#endif
