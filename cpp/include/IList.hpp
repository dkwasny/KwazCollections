#ifndef _CPP_KWAZ_ILIST_
#define _CPP_KWAZ_ILIST_

#include <cstddef>
#include "ICollection.hpp"

namespace KwazCollections
{
	class IList : public ICollection
	{
		public:
			virtual ~IList() {};

			virtual int& get(const size_t pIndex) const = 0;
			virtual int remove(const size_t pIndex) = 0;

			int& operator[](const size_t pIndex) const
			{
				return get(pIndex);
			};
	};
}

#endif
