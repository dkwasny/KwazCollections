#ifndef _C_KWAZ_IITERATOR_
#define _C_KWAZ_IITERATOR_

#include "Boolean.h"

typedef struct IIterator IIterator;

struct IIterator
{
	/* Methods */
	Boolean (* const hasNext)(IIterator* pIter);
	void* (* const peekNext)(IIterator* pIter);	
	void* (* const next)(IIterator* pIter);

	void (* const remove)(IIterator* pIter);
	
	void (* const destroy)(IIterator* pIter);

	/* Internal References */
	void* impl;
};

#endif
