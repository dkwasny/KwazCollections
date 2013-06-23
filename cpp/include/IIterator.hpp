#ifndef _CPP_KWAZ_IITERATOR_
#define _CPP_KWAZ_IITERATOR_

namespace KwazCollections
{
	class IIterator
	{
		public:
			virtual ~IIterator() {};
			
			virtual bool hasNext() = 0;
			virtual int& peekNext() = 0;
			virtual int& next() = 0;
	};
}

#endif
