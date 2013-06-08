#ifndef _KWAZ_ILIST_
#define _KWAZ_ILIST_

#include "ICollection.h"

typedef struct IList
{
	/* Super Interface Methods */
	int (* const getSize)(const struct IList* pList);
	void (* const destroy)(struct IList* pCollection);
	void (* const add)(struct IList* pList, const int pValue);
	
	/* Interface Methods */
	int (* const get)(const struct IList* pList, const size_t pIndex);
	int (* const remove)(struct IList* pList, const size_t pIndex);

	/* Internal References */
	ICollection* superType;
	void* subType;
} IList;

#endif
