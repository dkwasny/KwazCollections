static const size_t DEFAULT_CAPACITY = 10;
static const unsigned int DEFAULT_ADD_REALLOCATION_MULTIPLIER = 2;
static const unsigned int DEFAULT_REMOVE_REALLOCATION_THRESHOLD = 4;
static const unsigned int DEFAULT_REMOVE_REALLOCATION_DIVISOR = 2;

template <typename T>
ArrayList<T>::ArrayList() :
	initialCapacity(DEFAULT_CAPACITY),
	addReallocationMultiplier(DEFAULT_ADD_REALLOCATION_MULTIPLIER),
	removeReallocationThreshold(DEFAULT_REMOVE_REALLOCATION_THRESHOLD),
	removeReallocationDivisor(DEFAULT_REMOVE_REALLOCATION_DIVISOR),
	values(new T[DEFAULT_CAPACITY]),
	size(0),
	capacity(DEFAULT_CAPACITY)
{}

template <typename T>
ArrayList<T>::ArrayList(
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor
	) :
	initialCapacity(pCapacity),
	addReallocationMultiplier(pAddReallocationMultiplier),
	/* Safety Dance:
 	 * Ensure that the remove reallocation threshold is NEVER lower
 	 * than the rempove reallocation threshold.
 	 * Set the threshold to double the divisor if this happens.
 	 */
	removeReallocationThreshold(
		pRemoveReallocationThreshold < pRemoveReallocationDivisor
			? pRemoveReallocationDivisor * 2
			: pRemoveReallocationThreshold
	),
	removeReallocationDivisor(pRemoveReallocationDivisor),
	values(new T[pCapacity]),
	size(0),
	capacity(pCapacity)
{}

template <typename T>
ArrayList<T>::ArrayList(const ArrayList<T>& pOther) :
	initialCapacity(pOther.capacity),
	addReallocationMultiplier(pOther.addReallocationMultiplier),
	removeReallocationThreshold(pOther.removeReallocationThreshold),
	removeReallocationDivisor(pOther.removeReallocationDivisor),
	values(
		allocateArray(
			pOther.values,
			pOther.size,
			pOther.capacity
		)
	),
	size(pOther.size),
	capacity(pOther.capacity)
{}

/* FunTip: Valgrind yelled at me when I tried to use the value of
 *         initialCapacity to initialize values and capacity.
 *
 *         It reported that capacity was uninitalized during the following
 *         add() invocations which leads me to think that initialCapcity
 *         was uninitalized when I used it to intialize capacity.
 *
 *         From what I can tell online, since initialCapacity is earlier
 *         than values and capacity in both the class declaration and
 *         initializer list, this SHOULD be valid.
 *
 *         Manually doing the declarations appears to of cleared up the error
 *         even though it is less 'maintainable'.
 */
template <typename T>
ArrayList<T>::ArrayList(const IList<T>& pOther) :
	initialCapacity(pOther.getSize() * addReallocationMultiplier),
	addReallocationMultiplier(DEFAULT_ADD_REALLOCATION_MULTIPLIER),
	removeReallocationThreshold(DEFAULT_REMOVE_REALLOCATION_THRESHOLD),
	removeReallocationDivisor(DEFAULT_REMOVE_REALLOCATION_DIVISOR),
	values(new T[pOther.getSize() * addReallocationMultiplier]),
	size(0),
	capacity(pOther.getSize() * addReallocationMultiplier)
{
	for (size_t i = 0; i < pOther.getSize(); ++i)
	{
		add(pOther[i]);
	}
}

template <typename T>
ArrayList<T>::~ArrayList()
{
	delete[] values;
}

template <typename T>
ArrayList<T>& ArrayList<T>::operator=(const ArrayList<T>& pOther)
{
	if (this != &pOther)
	{
		values = allocateArray(
			pOther.values,
			pOther.capacity,
			pOther.size
		);	
		size = pOther.size;
		capacity = pOther.capacity;
	}

	return *this;
}

template <typename T>
size_t ArrayList<T>::getSize() const
{
	return size;	
}

template <typename T>
size_t ArrayList<T>::getCapacity() const
{
	return capacity;
}

template <typename T>
void ArrayList<T>::add(const T pValue)
{
	// Expand the array if we pass a specified threshold
	if (capacity == size)
	{
		size_t newCapacity = capacity * addReallocationMultiplier;
		// Gotta handle 0...
		if (newCapacity == 0)
		{
			newCapacity++;
		}
		int* newValues = allocateArray(values, size, newCapacity);
		delete[] values;
		values = newValues;
		capacity = newCapacity;
	}
	
	values[size++] = pValue;
}

template <typename T>
IIterator<T>* ArrayList<T>::iterator()
{
	return new Iterator(*this);
}

template <typename T>
T ArrayList<T>::remove(const size_t pIndex)
{
	if (pIndex >= size)
	{
		// TODO: Deal with sprintf later...pain in the...
		string msg = "Exception during ArrayList remove";
		throw IndexOutOfBoundsError(msg);
	}

	T removedVal = values[pIndex];
	
	--size;
	for (size_t i = pIndex; i < size; ++i)
	{
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
		T* newValues = allocateArray(values, size, newCapacity);
		delete[] values;
		values = newValues;
		capacity = newCapacity;
	}

	return removedVal;	
}

template <typename T>
T& ArrayList<T>::get(size_t pIndex) const
{
	if (pIndex >= size)
	{
		// TODO: Deal with sprintf later...pain in the...
		string msg = "Exception during ArrayList get";
		throw IndexOutOfBoundsError(msg);
	}

	return values[pIndex];
}

template <typename T>
T* ArrayList<T>::allocateArray(
	const T* pOrigValues,
	const size_t pOrigValuesSize,
	const size_t pNewCapacity) const
{
	if (pOrigValuesSize > pNewCapacity) {
		// TODO: Throw reallocation error or something
	}

	T* retVal = new T[pNewCapacity];
	size_t origValuesBytes = sizeof(int) * pOrigValuesSize;
	memcpy(retVal, pOrigValues, origValuesBytes);	

	return retVal;
}

template <typename T>
ArrayList<T>::Iterator::Iterator(const ArrayList<T>& pImpl) :
	nextIndex(0),
	impl(pImpl)
{}

template <typename T>
bool ArrayList<T>::Iterator::hasNext()
{
	bool retVal = false;

	if (nextIndex < impl.getSize())
	{
		retVal = true;
	}

	return retVal;
}

template <typename T>
T& ArrayList<T>::Iterator::peekNext()
{
	if (!hasNext())
	{
		string msg = "No more elements in ArrayList::Iterator::peekNext()";
		throw new NoMoreElementsError(msg);
	}
	
	return impl.get(nextIndex);
}

template <typename T>
T& ArrayList<T>::Iterator::next()
{
	if (!hasNext())
	{
		string msg = "No more elements in ArrayList::Iterator::next()";	
		throw new NoMoreElementsError(msg);
	}

	return impl.get(nextIndex++);
}
