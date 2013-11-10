#ifndef _CPP_KWAZ_ILIST_
#define _CPP_KWAZ_ILIST_

#include <cstddef>
#include "ICollection.hpp"

namespace KwazCollections
{
	template <typename T>
	class IList : public ICollection<T>
	{
		public:
			virtual ~IList() {};

			virtual T& get(const size_t pIndex) const = 0;
			virtual void remove(const size_t pIndex) = 0;

			T& operator[](const size_t pIndex) const
			{
				return get(pIndex);
			};
	};
}

#endif
