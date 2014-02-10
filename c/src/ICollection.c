#include "ICollection.h"

void ICollection_addAll(ICollection* pCollection, ICollection* pOtherCollection)
{
	IIterator* iter = pOtherCollection->iterator(pOtherCollection);
	while (iter->hasNext(iter))
	{
		pCollection->add(pCollection, iter->next(iter));
	}
	iter->destroy(iter);
}
