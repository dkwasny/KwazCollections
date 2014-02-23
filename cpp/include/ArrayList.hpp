#ifndef _CPP_KWAZ_ARRAY_LIST_
#define _CPP_KWAZ_ARRAY_LIST_

#include "IList.hpp"
#include "IndexOutOfBoundsError.hpp"
#include "NoMoreElementsError.hpp"
#include <cstring>

namespace KwazCollections
{
	template <typename T>
	class ArrayList : public IList<T>
	{
		public:
			ArrayList();
			ArrayList(
				const size_t pCapacity,
				const unsigned int pAddReallocationMultiplier,
				const unsigned int pRemoveReallocationThreshold,
				const unsigned int pRemoveReallocationDivisor
			);
			ArrayList(const ArrayList<T>& pOther);
			ArrayList(const IList<T>& pOther);
			~ArrayList();
			
			ArrayList<T>& operator=(const ArrayList<T>& pOther);
		
			// ICollection Methods
			size_t getSize() const;
			void add(const T& pValue);
			IIterator<T>* iterator();
			
			// IList Methods
			T& get(const size_t pIndex) const;
			void remove(const size_t pIndex);
			
			// ArrayList Methods
			size_t getCapacity() const;
		
		private:
			// Configuration constants
			const size_t initialCapacity;
			const unsigned int addReallocationMultiplier;
			const unsigned int removeReallocationThreshold;
			const unsigned int removeReallocationDivisor;
			
			// Mutable values
			T* values;
			size_t size;
			size_t capacity;

			// Internal methods
			T* allocateArray(
				const T* pOrigValues,
				const size_t pOrigValuesSize,
				const size_t pNewCapacity
			) const;

			// Private Iterator Implementation
			class Iterator : public IIterator<T>
			{
				public:
					Iterator(const ArrayList<T>& pImpl);
					
					bool hasNext();
					T& peekNext();
					T& next();

				private:
					size_t nextIndex;
					const ArrayList<T>& impl;
			};
	};
	
	#include "ArrayList.tpp"

}

#endif
