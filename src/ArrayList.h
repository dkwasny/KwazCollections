#ifndef _KWAZ_ARRAY_LIST_
#define _KWAZ_ARRAY_LIST_

// Default configuration values
#define ARRAY_LIST_INITIAL_CAPACITY 10
#define ARRAY_LIST_ADD_REALLOCATION_THRESHOLD 0.75f
#define ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER 2
#define ARRAY_LIST_REMOVE_REALLOCATION_THRESHOLD 0.25f
#define ARRAY_LIST_REMOVE_REALLOCATION_MULTIPLIER 0.5f

#include "IList.h"
#include <cstring>

class ArrayList : public IList {
	public:
		ArrayList();
		ArrayList(
			const size_t pCapacity,
        		const float pAddReallocationThreshold,
        		const unsigned short pAddReallocationMultiplier,
        		const float pRemoveReallocationThreshold,
        		const float pRemoveReallocationMultiplier
		);
		ArrayList(const ArrayList& pOther);
		ArrayList(const IList& pOther);
		~ArrayList();
		
		ArrayList& operator=(const ArrayList& pOther);
	
		size_t getSize() const;
		// This shouldn't be here but i need it for testing right now.
		size_t getCapacity() const;
		int& get(const size_t pIndex) const;
		void add(const int pValue);
		int remove(const size_t pIndex);
	
	private:
		// Configuration constants
		const size_t initialCapacity;
		const float addReallocationThreshold;
		const unsigned short addReallocationMultiplier;
		const float removeReallocationThreshold;
		const float removeReallocationMultiplier;
		
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
