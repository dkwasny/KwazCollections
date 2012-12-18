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
		void remove(const unsigned int pIndex);
	
	private:
		static const unsigned int DEFAULT_CAPACITY = 10;
		int* values;
		unsigned int size;
		unsigned int capacity;
};

#endif
