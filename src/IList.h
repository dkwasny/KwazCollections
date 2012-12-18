#ifndef _I_LIST_
#define _I_LIST_

class IList {
	public:
		virtual ~IList() {};
		//virtual IList& operator=(const IList& other) = 0;

		virtual unsigned int getSize() const = 0;
		virtual int& get(const unsigned int pIndex) const = 0;
		virtual void add(const int pValue) = 0;
		virtual void remove(const unsigned int pIndex) = 0;

		int& operator[](const unsigned int pIndex) { return get(pIndex); };
		const int& operator[](const unsigned int pIndex) const { return get(pIndex); };
};

#endif
