#ifndef _CPP_KWAZ_IITERATOR_
#define _CPP_KWAZ_IITERATOR_

namespace KwazCollections
{
	template <typename T>
	class IIterator
	{
		public:
			virtual ~IIterator() {};
			
			virtual bool hasNext() = 0;
			virtual T& peekNext() = 0;
			virtual T& next() = 0;
			virtual void remove() = 0;
	};
}

#endif
