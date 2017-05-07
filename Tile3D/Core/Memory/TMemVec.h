#pragma once


//TBD
//1) Test fixed size(ex. 255) vs dynamic size
//
//
template<class T>
class TMemoryVec
{
public:
	TMemoryVec() : m_pData(NULL), m_size(0), m_maxSize(0) {}
	~TMemoryVec() {
		if (m_pData) {
			RawMemFree(m_pData);
		}
	}

public:
	void Push(const T & t) {
		if (m_size >= m_maxSize) {
			int newSize = (m_maxSize + 16) * 3 / 2;
			T * pNewData = (T*)RawMemAlloc(newSize * sizeof(T));

			if (m_pData) {
				memcpy(pNewData, m_pData, m_size*sizeof(T));
				RawMemFree(m_pData);
			}
			m_pData = pNewData;
			m_maxSize = newSize;
		}

		m_pData[m_size++] = t;
	}

	const T & operator[] (int n) const
	{
		TAssert(n >= 0 && n < m_size);
		return m_pData[n];
	}

	void RemoveQuick(int n) {
		TAssert(n >= 0 && n < m_size);
		if (n == m_size) {
			m_size--;
		}
		else {
			m_pData[n] = m_pData[m_size - 1];
			m_size--;
		}
	}

	int Size() const { return m_size; }
	int MaxSize() const { return m_maxSize; }


protected:
	T * m_pData;
	int m_size;
	int m_maxSize;
};

