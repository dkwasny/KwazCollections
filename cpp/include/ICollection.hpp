#ifndef _CPP_KWAZ_ICOLLECTION_
#define _CPP_KWAZ_ICOLLECTION_

#include "IIterator.hpp"

namespace KwazCollections
{
	template <typename T>
	class ICollection
	{
		public:
			virtual ~ICollection() {};
			
			virtual size_t getSize() const = 0;
			virtual void add(const T pValue) = 0;
			virtual IIterator<T>* iterator() = 0;
	};
}

#endif
