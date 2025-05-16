#pragma once
template<typename T>
class CUtils
{
public:
	CUtils(int sizeX, int sizeY) : m_iXSize(sizeX), m_iYSize(sizeY)	
												{ m_pArray = new T[sizeY * sizeX;]					}
	~CUtils()									{ delete[] m_pArray; m_pArray = nullptr;			}
	T& operator()(int indexX, int indexY)		{ return	m_pArray[indexY * m_iXSize + indexX];	}
	const T& operator()(int indexX, int indexY)	{ return	m_pArray[indexY * m_iXSize + indexX];	}
private:
	T* m_pArray;
	const int m_iYSize;
	const int m_iXSize;
};

