#include "ArrayList.hpp"
#include "IndexOutOfBoundsError.hpp"

ArrayList::ArrayList() :
	initialCapacity(ARRAY_LIST_INITIAL_CAPACITY),
	addReallocationMultiplier(ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER),
	removeReallocationThreshold(ARRAY_LIST_REMOVE_REALLOCATION_THRESHOLD),
	removeReallocationDivisor(ARRAY_LIST_REMOVE_REALLOCATION_DIVISOR)
{
	size = 0;
	capacity = initialCapacity;
	values = new int[capacity];
}

ArrayList::ArrayList(
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor
	) :
	initialCapacity(pCapacity),
	addReallocationMultiplier(pAddReallocationMultiplier),
	removeReallocationThreshold(pRemoveReallocationThreshold),
	removeReallocationDivisor(pRemoveReallocationDivisor)
{
	size = 0;
	capacity = pCapacity;
	values = new int[capacity];
}

ArrayList::ArrayList(const ArrayList& pOther) :
	initialCapacity(pOther.capacity),
	addReallocationMultiplier(pOther.addReallocationMultiplier),
	removeReallocationThreshold(pOther.removeReallocationThreshold),
	removeReallocationDivisor(pOther.removeReallocationDivisor)
{
	values = allocateArray(pOther.values, pOther.size, pOther.capacity);
	size = pOther.size;
	capacity = pOther.capacity;
}

ArrayList::ArrayList(const IList& pOther) :
	initialCapacity(ARRAY_LIST_INITIAL_CAPACITY),
	addReallocationMultiplier(ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER),
	removeReallocationThreshold(ARRAY_LIST_REMOVE_REALLOCATION_THRESHOLD),
	removeReallocationDivisor(ARRAY_LIST_REMOVE_REALLOCATION_DIVISOR)
{
	size = 0;
	capacity = pOther.getSize() * addReallocationMultiplier;
	values = new int[capacity];
	for (size_t i = 0; i < pOther.getSize(); ++i) {
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

size_t ArrayList::getSize() const {
	return size;	
}

void ArrayList::add(const int pValue) {
	// Expand the array if we pass a specified threshold
	if (capacity == size) {
		size_t newCapacity = capacity * addReallocationMultiplier;
		if (newCapacity == 0) { newCapacity++; }  // Gotta handle 0...
		int* newValues = allocateArray(values, size, newCapacity);
		delete[] values;
		values = newValues;
		capacity = newCapacity;
	}
	
	values[size++] = pValue;
}

int ArrayList::remove(const size_t pIndex) {	
	if (pIndex >= size) {
		// TODO: Deal with sprintf later...pain in the...
		string msg = "Exception during ArrayList remove";
		throw IndexOutOfBoundsError(msg);
	}

	int removedVal = values[pIndex];
	
	--size;
	for (size_t i = pIndex; i < size; ++i) {
		values[i] = values[i + 1];
	}
	
	// Shrink the array if we are only using a small portion of it.
	// To prevent non-stop resizing, the array will not be shrunk
	// past its initial capacity.
	// NERD ALERT: This floating point arithmetic could slow things down
	//             but the potential memory savings is better IMO until
	//             proven otherwise.
	size_t newCapacity = capacity / removeReallocationDivisor;
	if (newCapacity >= initialCapacity && (size * removeReallocationThreshold) < capacity)
	{
		int* newValues = allocateArray(values, size, newCapacity);
		delete[] values;
		values = newValues;
		capacity = newCapacity;
	}

	return removedVal;	
}

int& ArrayList::get(size_t pIndex) const {
	if (pIndex >= size) {
		// TODO: Deal with sprintf later...pain in the...
		string msg = "Exception during ArrayList get";
		throw IndexOutOfBoundsError(msg);
	}

	return values[pIndex];
}

int* ArrayList::allocateArray(
	const int* pOrigValues,
	const size_t pOrigValuesSize,
	const size_t pNewCapacity) const
{
	if (pOrigValuesSize > pNewCapacity) {
		// TODO: Throw reallocation error or something
	}

	int* retVal = new int[pNewCapacity];
	size_t origValuesBytes = sizeof(int) * pOrigValuesSize;
	memcpy(retVal, pOrigValues, origValuesBytes);	

	return retVal;
}
