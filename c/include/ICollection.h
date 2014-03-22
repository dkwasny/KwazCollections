#ifndef _C_KWAZ_ICOLLECTION_
#define _C_KWAZ_ICOLLECTION_

#include <stdlib.h>
#include "IIterator.h"
#include "Boolean.h"

typedef struct ICollection ICollection;

struct ICollection
{
	/* Interface Methods */
	size_t (* const getSize)(const ICollection* pCollection);
	void (* const destroy)(ICollection* pCollection);
	void (* const add)(ICollection* pCollection, const void* pValue);
	void (* const addAll)(ICollection* pCollection, ICollection* pOtherCollection);
	Boolean (* const contains)(ICollection* pCollection, const void* pValue);
	IIterator* (* const iterator)(ICollection* pCollection);

	/* Internal References */
	void* subType;
};
	
/* Provided Implementations */
void ICollection_addAll(ICollection* pCollection, ICollection* pOtherCollection);

#endif
