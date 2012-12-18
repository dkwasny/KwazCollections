#include "ArrayList.h"
#include "IndexOutOfBoundsError.h"

ArrayList::ArrayList() {
	values = new int[DEFAULT_SIZE];
	size = DEFAULT_SIZE;
}

ArrayList::~ArrayList() {
	delete[] values;
}

int& ArrayList::operator[](const unsigned int index) {
	return get(index);
}

const int& ArrayList::operator[](unsigned int index) const {
	return get(index);
}

int& ArrayList::get(unsigned int index) const {
	if (index >= size) {
		string msg = "TODO: Better Error Message";
		throw IndexOutOfBoundsError(msg);
	}

	return values[index];
}
