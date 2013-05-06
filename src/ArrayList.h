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
			const unsigned int pCapacity,
			const unsigned int pInitialCapacity,
        		const float pAddReallocationThreshold,
        		const unsigned short pAddReallocationMultiplier,
        		const float pRemoveReallocationThreshold,
        		const float pRemoveReallocationMultiplier
		);
		ArrayList(const ArrayList& pOther);
		ArrayList(const IList& pOther);
		~ArrayList();
		
		ArrayList& operator=(const ArrayList& pOther);
	
		unsigned int getSize() const;
		// This shouldn't be here but i need it for testing right now.
		unsigned int getCapacity() const;
		int& get(const unsigned int pIndex) const;
		void add(const int pValue);
		int remove(const unsigned int pIndex);
	
	private:
		// Configuration constants
		const unsigned int initialCapacity;
		const float addReallocationThreshold;
		const unsigned int addReallocationMultiplier;
		const float removeReallocationThreshold;
		const float removeReallocationMultiplier;
		
		// Mutable values
		int* values;
		unsigned int size;
		unsigned int capacity;

		// Internal methods
		int* allocateArray(
			const int* pOrigValues,
			const unsigned int pOrigValuesSize,
			const unsigned int pNewCapacity
		) const;
};

#endif
