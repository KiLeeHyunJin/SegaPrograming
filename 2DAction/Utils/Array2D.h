
template< class T > class Array2D
{
public:
	Array2D() : mArray(0) {}
	Array2D(int index0, int index1) : mSize0(index0), mSize1(index1)
	{
		mArray = new T[index0 * index1];
	}
	~Array2D()
	{
		delete[] mArray;
		mSize0 = mSize1 = 0;
	}

	void SetSize(int index0, int index1)
	{
		mSize0 = index0;
		mSize1 = index1;
		mArray = new T[index0 * index1];
	}

	const T& operator()(int index0, int index1) const
	{
		return mArray[index0 * mSize0 + index1];
	}

	T& operator() (int index0, int index1) 
	{
		return mArray[index0 * mSize0 + index1];
	}

private:
	T* mArray;
	int mSize0;
	int mSize1;
};