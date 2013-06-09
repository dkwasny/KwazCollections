#ifndef _CPP_KWAZ_ARRAY_LIST_
#define _CPP_KWAZ_ARRAY_LIST_

#include "IList.hpp"
#include <cstring>

class ArrayList : public IList {
	public:
		ArrayList();
		ArrayList(
			const size_t pCapacity,
        		const unsigned int pAddReallocationMultiplier,
        		const unsigned int pRemoveReallocationThreshold,
        		const unsigned int pRemoveReallocationDivisor
		);
		ArrayList(const ArrayList& pOther);
		ArrayList(const IList& pOther);
		~ArrayList();
		
		ArrayList& operator=(const ArrayList& pOther);
	
		size_t getSize() const;
		int& get(const size_t pIndex) const;
		void add(const int pValue);
		int remove(const size_t pIndex);
	
	private:
		// Configuration constants
		const size_t initialCapacity;
		const unsigned int addReallocationMultiplier;
		const unsigned int removeReallocationThreshold;
		const unsigned int removeReallocationDivisor;
		
		// Mutable values
		int* values;
		size_t size;
		size_t capacity;

		// Internal methods
		int* allocateArray(
			const int* pOrigValues,
			const size_t pOrigValuesSize,
			const size_t pNewCapacity
		) const;
};

#endif
