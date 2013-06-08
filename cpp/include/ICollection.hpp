class ICollection {
	public:
		virtual ~ICollection() {};
		
		virtual size_t getSize() const = 0;
		virtual void add(const int pValue) = 0;
};
