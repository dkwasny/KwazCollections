#ifndef _C_KWAZ_IITERATOR_
#define _C_KWAZ_IITERATOR_

#include "Boolean.h"

typedef struct IIterator IIterator;

struct IIterator
{
	/* Methods */
	Boolean (* const hasNext)(IIterator* pIter);
	int (* const peekNext)(IIterator* pIter);	
	int (* const next)(IIterator* pIter);
	
	int (* const current)(IIterator* pIter);
	
	Boolean (* const hasPrevious)(IIterator* pIter);
	int (* const peekPrevious)(IIterator* pIter);
	int (* const previous)(IIterator* pIter);
	
	void (* const destroy)(IIterator* pIter);

	/* Internal References */
	void* impl;
};

#endif
