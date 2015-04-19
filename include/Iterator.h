#ifndef _C_KWAZ_Iterator_
#define _C_KWAZ_Iterator_

#include "Boolean.h"

typedef struct Iterator Iterator;

struct Iterator
{
	/* Methods */
	Boolean (* const hasNext)(Iterator* pIter);
	void* (* const peekNext)(Iterator* pIter);	
	void* (* const next)(Iterator* pIter);

	void (* const remove)(Iterator* pIter);
	
	void (* const destroy)(Iterator* pIter);

	/* Internal References */
	void* impl;
};

#endif
