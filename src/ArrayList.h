#ifndef _KWAZ_ARRAY_LIST_
#define _KWAZ_ARRAY_LIST_

class ArrayList {
	public:
		ArrayList();
		~ArrayList();

		int& operator[](const unsigned int index);
		const int& operator[](const unsigned int index) const;
	
	private:
		static const int DEFAULT_SIZE = 10;
		int* values;
		int size;
};

#endif
