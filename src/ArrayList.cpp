#include "ArrayList.h"
#include "IndexOutOfBoundsError.h"

ArrayList::ArrayList() {
	size = 0;
	capacity = DEFAULT_CAPACITY;
	values = new int[capacity];
}

ArrayList::ArrayList(const unsigned int pCapacity) {
	size = 0;
	capacity = pCapacity;
	values = new int[capacity];
}

ArrayList::~ArrayList() {
	delete[] values;
}

ArrayList::ArrayList(const ArrayList& pOther) {
	size = pOther.size;
	capacity = pOther.capacity;
	values = new int[capacity];
	for(int i = 0; i < size; ++i) {
		values[i] = pOther.values[i];
	}
}

ArrayList& ArrayList::operator=(const ArrayList& pOther) {
	if (this != &pOther) {
		size = pOther.size;
		capacity = pOther.capacity;
		int* newValues = new int[capacity];
		
		for(int i = 0; i < size; ++i) {
			newValues[i] = pOther.values[i];
		}
		
		delete[] values;
		values = newValues;
	}

	return *this;
}

unsigned int ArrayList::getSize() const {
	return size;	
}

void ArrayList::add(const int pValue) {
	// TODO: Resize when we have too many values
	values[size++] = pValue;
}

void ArrayList::remove(const unsigned int pIndex) {
	// TODO: Something meaningful
}

int& ArrayList::get(unsigned int pIndex) const {
	if (pIndex >= size) {
		string msg = "TODO: Better Error Message";
		throw IndexOutOfBoundsError(msg);
	}

	return values[pIndex];
}
