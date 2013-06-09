#ifndef _C_KWAZ_ICOLLECTION_
#define _C_KWAZ_ICOLLECTION_

typedef struct ICollection
{
	/* Interface Methods */
	int (* const getSize)(const struct ICollection* pCollection);
	void (* const destroy)(struct ICollection* pCollection);
	void (* const add)(struct ICollection* pCollection, const int pValue);

	/* Internal References */
	void* subType;
} ICollection;

#endif
