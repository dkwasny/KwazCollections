#ifndef _KWAZ_ARRAY_LIST_
#define _KWAZ_ARRAY_LIST_

#include "IList.h"

class ArrayList : public IList {
	public:
		ArrayList();
		ArrayList(const unsigned int pCapacity);
		~ArrayList();
		ArrayList(const ArrayList& pOther);
		
		ArrayList& operator=(const ArrayList& pOther);
	
		unsigned int getSize() const;
		int& get(const unsigned int pIndex) const;
		void add(const int pValue);
		int remove(const unsigned int pIndex);
	
	private:
		const unsigned int INITIAL_CAPACITY;
		const float ADD_REALLOCATION_THRESHOLD;
		static const unsigned int ADD_REALLOCATION_MULTIPLIER = 2;
		const float REMOVE_REALLOCATION_THRESHOLD;
		const float REMOVE_REALLOCATION_MULTIPLIER;
		int* values;
		unsigned int size;
		unsigned int capacity;

		int* allocateArray(
			const int* pOrigValues,
			const unsigned int pOrigValuesSize,
			const unsigned int pNewCapacity
		) const;
};

#endif
