template <typename T>
void ICollection<T>::addAll(ICollection<T>& pCollection)
{
	IIterator<T>* iter = pCollection.iterator();
	
	while (iter->hasNext())
	{
		add(iter->next());
	}

	delete iter;
}
