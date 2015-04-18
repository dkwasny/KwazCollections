#ifndef _C_KWAZ_ARRAY_LIST_
#define _C_KWAZ_ARRAY_LIST_

#include <stdlib.h>
#include <string.h>
#include "IList.h"
#include "ArrayListImpl.h"
#include "IIterator.h"
#include "Boolean.h"

/* The public interface structure.
 */
typedef struct ArrayList ArrayList;

struct ArrayList
{
	/* ICollection Methods */
	size_t (* const getSize)(const ArrayList* pList);
	void (* const destroy)(ArrayList* pList);
	void (* const add)(ArrayList* pList, const void* pValue);
	void (* const addAll)(ArrayList* pList, ICollection* pOtherCollection);
	Boolean (* const contains)(ArrayList* pList, const void* pValue);
	IIterator* (* const iterator)(ArrayList* pList);

	/* IList Methods */
	void* (* const get)(const ArrayList* pList, const size_t pIndex);
	void (* const remove)(ArrayList* pList, const size_t pIndex);

	/* ArrayList Methods */
	size_t (* const getCapacity)(const ArrayList* pList);

	IList* superType;
	ArrayListImpl* impl;
};

/* Start extern for c++ */
#ifdef __cplusplus
extern "C" {
#endif

/* Constructors */
ArrayList* ArrayList_createDefault(const size_t pTypeSize);
ArrayList* ArrayList_create(
	const size_t pTypeSize,
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor
);
ArrayList* ArrayList_createCompare(
	const size_t pTypeSize,
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor,
	int (* pCompare)(const void* first, const void* second, size_t size)
);

/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
