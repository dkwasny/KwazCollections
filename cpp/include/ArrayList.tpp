static const size_t DEFAULT_CAPACITY = 10;
static const unsigned int DEFAULT_ADD_REALLOCATION_MULTIPLIER = 2;
static const unsigned int DEFAULT_REMOVE_REALLOCATION_THRESHOLD = 4;
static const unsigned int DEFAULT_REMOVE_REALLOCATION_DIVISOR = 2;

ArrayList::ArrayList() :
	initialCapacity(DEFAULT_CAPACITY),
	addReallocationMultiplier(DEFAULT_ADD_REALLOCATION_MULTIPLIER),
	removeReallocationThreshold(DEFAULT_REMOVE_REALLOCATION_THRESHOLD),
	removeReallocationDivisor(DEFAULT_REMOVE_REALLOCATION_DIVISOR),
	values(new int[DEFAULT_CAPACITY]),
	size(0),
	capacity(DEFAULT_CAPACITY)
{}

ArrayList::ArrayList(
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
	values(new int[pCapacity]),
	size(0),
	capacity(pCapacity)
{}

ArrayList::ArrayList(const ArrayList& pOther) :
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
ArrayList::ArrayList(const IList& pOther) :
	initialCapacity(pOther.getSize() * addReallocationMultiplier),
	addReallocationMultiplier(DEFAULT_ADD_REALLOCATION_MULTIPLIER),
	removeReallocationThreshold(DEFAULT_REMOVE_REALLOCATION_THRESHOLD),
	removeReallocationDivisor(DEFAULT_REMOVE_REALLOCATION_DIVISOR),
	values(new int[pOther.getSize() * addReallocationMultiplier]),
	size(0),
	capacity(pOther.getSize() * addReallocationMultiplier)
{
	for (size_t i = 0; i < pOther.getSize(); ++i)
	{
		add(pOther[i]);
	}
}

ArrayList::~ArrayList()
{
	delete[] values;
}

ArrayList& ArrayList::operator=(const ArrayList& pOther)
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

size_t ArrayList::getSize() const
{
	return size;	
}

size_t ArrayList::getCapacity() const
{
	return capacity;
}

void ArrayList::add(const int pValue)
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

IIterator* ArrayList::iterator()
{
	return new Iterator(*this);
}

int ArrayList::remove(const size_t pIndex)
{
	if (pIndex >= size)
	{
		// TODO: Deal with sprintf later...pain in the...
		string msg = "Exception during ArrayList remove";
		throw IndexOutOfBoundsError(msg);
	}

	int removedVal = values[pIndex];
	
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
		int* newValues = allocateArray(values, size, newCapacity);
		delete[] values;
		values = newValues;
		capacity = newCapacity;
	}

	return removedVal;	
}

int& ArrayList::get(size_t pIndex) const
{
	if (pIndex >= size)
	{
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

ArrayList::Iterator::Iterator(const ArrayList& pImpl) :
	nextIndex(0),
	impl(pImpl)
{}

bool ArrayList::Iterator::hasNext()
{
	bool retVal = false;

	if (nextIndex < impl.getSize())
	{
		retVal = true;
	}

	return retVal;
}

int& ArrayList::Iterator::peekNext()
{
	if (!hasNext())
	{
		string msg = "No more elements in ArrayList::Iterator::peekNext()";
		throw new NoMoreElementsError(msg);
	}
	
	return impl.get(nextIndex);
}

int& ArrayList::Iterator::next()
{
	if (!hasNext())
	{
		string msg = "No more elements in ArrayList::Iterator::next()";	
		throw new NoMoreElementsError(msg);
	}

	return impl.get(nextIndex++);
}
