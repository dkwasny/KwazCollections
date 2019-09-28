#ifndef _C_KWAZ_GenericIterator_
#define _C_KWAZ_GenericIterator_

#include "Boolean.h"

typedef struct GenericIterator GenericIterator;

struct GenericIterator
{
	/* Methods */
	Boolean (* const hasNext)(const GenericIterator* pIter);
	void* (* const peekNext)(const GenericIterator* pIter);	
	void* (* const next)(GenericIterator* pIter);

	void (* const remove)(GenericIterator* pIter);
	
	void (* const destroy)(GenericIterator* pIter);

	/* Internal References */
	void* impl;
};

#endif
