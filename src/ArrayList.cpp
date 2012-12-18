#include "ArrayList.h"
#include "IndexOutOfBoundsError.h"
#include <cstdio>

ArrayList::ArrayList() :
	ADD_REALLOCATION_THRESHOLD(0.75),
	REMOVE_REALLOCATION_THRESHOLD(0.25),
	REMOVE_REALLOCATION_MULTIPLIER(0.5),
	INITIAL_CAPACITY(10)
{
	size = 0;
	capacity = INITIAL_CAPACITY;
	values = new int[capacity];
}

ArrayList::ArrayList(const unsigned int pCapacity) :
	ADD_REALLOCATION_THRESHOLD(0.75),
	REMOVE_REALLOCATION_THRESHOLD(0.25),
	REMOVE_REALLOCATION_MULTIPLIER(0.5),
	INITIAL_CAPACITY(pCapacity)
{
	size = 0;
	capacity = pCapacity;
	values = new int[capacity];
}

ArrayList::ArrayList(const ArrayList& pOther) : 
	ADD_REALLOCATION_THRESHOLD(0.75),
	REMOVE_REALLOCATION_THRESHOLD(0.25),
	REMOVE_REALLOCATION_MULTIPLIER(0.5),
	INITIAL_CAPACITY(pOther.capacity)
{
	int* values = allocateArray(pOther.values, pOther.size, pOther.capacity);
	size = pOther.size;
	capacity = pOther.capacity;
}

ArrayList::~ArrayList() {
	delete[] values;
}

ArrayList& ArrayList::operator=(const ArrayList& pOther) {
	if (this != &pOther) {
		int* newValues = allocateArray(pOther.values, pOther.size, pOther.capacity);
		delete[] values;
		values = newValues;	
		
		size = pOther.size;
		capacity = pOther.capacity;
	}

	return *this;
}

unsigned int ArrayList::getSize() const {
	return size;	
}

void ArrayList::add(const int pValue) {
	// Expand the array if we pass a specified threshold
	if ((capacity * ADD_REALLOCATION_THRESHOLD) <= size) {
		int newCapacity = capacity * ADD_REALLOCATION_MULTIPLIER;
		if (newCapacity == 0) { newCapacity++; }  // Gotta handle 0...
		int* newValues = allocateArray(values, size, newCapacity);
		delete[] values;
		values = newValues;
		capacity = newCapacity;
	}
	
	values[size++] = pValue;

		printf("\nadding %d %d\n", capacity, size);
}

int ArrayList::remove(const unsigned int pIndex) {	
	if (pIndex >= size) {
		// TODO: Deal with sprintf later...pain in the...
		string msg = "Exception during ArrayList remove";
		throw IndexOutOfBoundsError(msg);
	}

	int removedVal = values[pIndex];
	
	--size;
	for (int i = pIndex; i < size; ++i) {
		values[i] = values[i + 1];
	}
	
		printf("\nSHRINGKING %d %d\n", capacity, size);
	// Shrink the array if we are only using a small portion of it.
	// To prevent non-stop resizing, the array will not be shrunk
	// past its initial capacity.
	// NERD ALERT: This floating point arithmetic could slow things down
	//             but the potential memory savings is better IMO until
	//             proven otherwise.
	if (capacity > INITIAL_CAPACITY && (capacity * REMOVE_REALLOCATION_THRESHOLD) >= size) {
		int newCapacity = capacity * REMOVE_REALLOCATION_MULTIPLIER;
		int* newValues = allocateArray(values, size, newCapacity);
		delete[] values;
		values = newValues;
		capacity = newCapacity;
	}

	return removedVal;	
}

int& ArrayList::get(unsigned int pIndex) const {
	if (pIndex >= size) {
		// TODO: Deal with sprintf later...pain in the...
		string msg = "Exception during ArrayList get";
		throw IndexOutOfBoundsError(msg);
	}

	return values[pIndex];
}

int* ArrayList::allocateArray(
	const int* pOrigValues,
	const unsigned int pOrigValuesSize,
	const unsigned int pNewCapacity) const
{
	if (pOrigValuesSize > pNewCapacity) {
		// TODO: Throw reallocation error or something
	}

	// TODO: Maybe theres some old C hoodoo (like memcpy) I can use here
	// for no other reason than being a nerd
	int* retVal = new int[pNewCapacity];	
	for(int i = 0; i < pOrigValuesSize; ++i) {
		retVal[i] = pOrigValues[i];
	}

	return retVal;
}
