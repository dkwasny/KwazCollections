#include "ArrayList.h"
#include "IndexOutOfBoundsError.h"

ArrayList::ArrayList() :
	initialCapacity(ARRAY_LIST_INITIAL_CAPACITY),
	addReallocationThreshold(ARRAY_LIST_ADD_REALLOCATION_THRESHOLD),
	addReallocationMultiplier(ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER),
	removeReallocationThreshold(ARRAY_LIST_REMOVE_REALLOCATION_THRESHOLD),
	removeReallocationMultiplier(ARRAY_LIST_REMOVE_REALLOCATION_MULTIPLIER)
{
	size = 0;
	capacity = initialCapacity;
	values = new int[capacity];
}

ArrayList::ArrayList(
	const unsigned int pCapacity,
	const float pAddReallocationThreshold,
	const unsigned short pAddReallocationMultiplier,
	const float pRemoveReallocationThreshold,
	const float pRemoveReallocationMultiplier
	) :
	initialCapacity(pCapacity),
	addReallocationThreshold(pAddReallocationThreshold),
	addReallocationMultiplier(pAddReallocationMultiplier),
	removeReallocationThreshold(pRemoveReallocationThreshold),
	removeReallocationMultiplier(pRemoveReallocationMultiplier)
{
	size = 0;
	capacity = pCapacity;
	values = new int[capacity];
}

ArrayList::ArrayList(const ArrayList& pOther) :
	initialCapacity(pOther.capacity),
	addReallocationThreshold(pOther.addReallocationThreshold),
	addReallocationMultiplier(pOther.addReallocationMultiplier),
	removeReallocationThreshold(pOther.removeReallocationThreshold),
	removeReallocationMultiplier(pOther.removeReallocationMultiplier)
{
	values = allocateArray(pOther.values, pOther.size, pOther.capacity);
	size = pOther.size;
	capacity = pOther.capacity;
}

ArrayList::ArrayList(const IList& pOther) :
	initialCapacity(ARRAY_LIST_INITIAL_CAPACITY),
	addReallocationThreshold(ARRAY_LIST_ADD_REALLOCATION_THRESHOLD),
	addReallocationMultiplier(ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER),
	removeReallocationThreshold(ARRAY_LIST_REMOVE_REALLOCATION_THRESHOLD),
	removeReallocationMultiplier(ARRAY_LIST_REMOVE_REALLOCATION_MULTIPLIER)
{
	size = 0;
	capacity = pOther.getSize() * addReallocationMultiplier;
	values = new int[capacity];
	for (unsigned int i = 0; i < pOther.getSize(); ++i) {
		add(pOther[i]);
	}
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

unsigned int ArrayList::getCapacity() const {
	return capacity;
}

void ArrayList::add(const int pValue) {
	// Expand the array if we pass a specified threshold
	if ((capacity * addReallocationThreshold) <= size) {
		int newCapacity = capacity * addReallocationMultiplier;
		if (newCapacity == 0) { newCapacity++; }  // Gotta handle 0...
		int* newValues = allocateArray(values, size, newCapacity);
		delete[] values;
		values = newValues;
		capacity = newCapacity;
	}
	
	values[size++] = pValue;
}

int ArrayList::remove(const unsigned int pIndex) {	
	if (pIndex >= size) {
		// TODO: Deal with sprintf later...pain in the...
		string msg = "Exception during ArrayList remove";
		throw IndexOutOfBoundsError(msg);
	}

	int removedVal = values[pIndex];
	
	--size;
	for (unsigned int i = pIndex; i < size; ++i) {
		values[i] = values[i + 1];
	}
	
	// Shrink the array if we are only using a small portion of it.
	// To prevent non-stop resizing, the array will not be shrunk
	// past its initial capacity.
	// NERD ALERT: This floating point arithmetic could slow things down
	//             but the potential memory savings is better IMO until
	//             proven otherwise.
	unsigned int newCapacity = capacity * removeReallocationMultiplier;
	if (newCapacity >= initialCapacity && (capacity * removeReallocationThreshold) >= size)
	{
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

	size_t valuesSize = sizeof(int) * pNewCapacity;
	int* retVal = new int[valuesSize];
	memcpy(retVal, pOrigValues, valuesSize);	

	return retVal;
}
