#pragma once
template<typename T>
class CArray
{
public:
	CArray(int sizeX, int sizeY) : m_iXSize(sizeX), m_iYSize(sizeY)
												{ m_pArray = new T[sizeY * sizeX]; }
	~CArray()									{ delete[] m_pArray; m_pArray = nullptr;			}
	T& operator()(int indexX, int indexY)		{ return	m_pArray[(indexY * m_iXSize) + indexX];	}
	const T& operator()(int indexX, int indexY) const	{ return	m_pArray[(indexY * m_iXSize) + indexX];	}
private:
	T* m_pArray;
	const int m_iYSize;
	const int m_iXSize;
};

class Flag
{
public :
	Flag() : m_cFlags(0){}
	~Flag() {}
	
	bool Check(short flag) const 
	{ 
		char temp = 1 << flag; 
		return m_cFlags & temp;
	}
	void Set(short flag) 
	{ 
		char temp = 1 << flag;  
		m_cFlags |= temp; 
	}
	void Reset(short flag) 
	{ 
		char temp = ~(1 << flag); 
		m_cFlags &= temp; 
	}

private:
	unsigned char m_cFlags;

};

