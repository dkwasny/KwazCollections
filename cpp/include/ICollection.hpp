#ifndef _CPP_KWAZ_ICOLLECTION_
#define _CPP_KWAZ_ICOLLECTION_

class ICollection {
	public:
		virtual ~ICollection() {};
		
		virtual size_t getSize() const = 0;
		virtual void add(const int pValue) = 0;
};

#endif
