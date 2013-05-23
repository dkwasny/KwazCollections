#ifndef _I_LIST_
#define _I_LIST_

#include <cstddef>

class IList {
	public:
		virtual ~IList() {};

		virtual size_t getSize() const = 0;
		virtual int& get(const size_t pIndex) const = 0;
		virtual void add(const int pValue) = 0;
		virtual int remove(const size_t pIndex) = 0;

		int& operator[](const size_t pIndex) { return get(pIndex); };
		const int& operator[](const size_t pIndex) const { return get(pIndex); };
};

#endif
