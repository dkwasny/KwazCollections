#ifndef _I_LIST_
#define _I_LIST_

#include <cstddef>
#include "ICollection.hpp"

class IList : public ICollection {
	public:
		virtual ~IList() {};

		virtual int& get(const size_t pIndex) const = 0;
		virtual int remove(const size_t pIndex) = 0;

		int& operator[](const size_t pIndex) { return get(pIndex); };
		const int& operator[](const size_t pIndex) const { return get(pIndex); };
};

#endif
