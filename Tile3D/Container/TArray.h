#pragma once

#include "Util/TAssert.h"

//
//1) if the type is pointer, how to manager the memory [manually manager or invoke ClearPtr] [done]
//3) move ctor? [done]
//4) support Serialize?
//5) inherit from TObject?
//5) support iterator?
//6) support swap?
//7) support sort?
template<typename T> class TArray
{
public:
	enum TARRAY_CONST
	{
		ARRAY_INIT_SIZE = 6,
		ARRAY_INDEX_NONE = -1,
	};

	TArray() {
		m_data = nullptr;
		m_count = 0;
		m_capacity = 0;
	}

	TArray(int n, const T & val) {
		Reserve(n);
		m_count = n;
		for (int i = 0; i < m_count; ++i) {
			m_data[i] = val;
		}
	}

	explicit TArray(TArray & arr) {
		m_count = arr.Size();
		m_capacity = arr.Capacity();
		m_data = new T[m_capacity];
		memcpy(m_data, arr.GetData(), m_count*sizeof(T));
	}

	~TArray() {
		Clear();
	}

	//Move constructor
	TArray(TArray&& arr) {
		m_data = arr.GetData();
		m_count = arr.Size();
		m_capacity = arr.Capacity();
		//be moved
		arr.SetData(nullptr);
	}

public:
	T & operator[](int index) {
		return m_data[index];
	}

	const T & operator[](int index) const {
		return m_data[index];
	}

	//Array Size
	int Size() const { return m_count; }

	//Capacity (Capacity >= Size)
	int Capacity() const { return m_capacity; }

	T* GetData() const { return m_data; }

	void SetData(T * data) {
		m_data = data;
	}

	bool IsEmpty() { return m_count == 0; }

	//Add the element at the tail
	void Add(const T & element);

	//Pop the element from the tail
	void Pop();

	//Insert the element at the position index
	void Insert(const T & element, int index);

	//Insert the element at the pos 
	void Insert(const T & element, const T * pos);

	//Remove the element
	void Remove(int index);

	//Remove the element
	void Remove(const T * pos);

	//Remove the element at once, implement it by the exchange the last elment to this position, disorder the array, simlar to the STL remove generic alogorithm
	void RemoveAtOnce(int index);

	//Find the element within the array and return the index if exist, if not exist return ARRAY_INDEX_NONE
	int Find(const T & element);

	//Check if the array is equal with another array
	bool Equal(const TArray<T> & arr);


	//Reserve the array list to count size
	void Reserve(int count);

	//Shrink the array list to count size
	void Shrink(int count);

	//Clear the Array
	void Clear() {
		if (m_data != nullptr) {
			delete[] m_data;
		}

		m_data = nullptr;
		m_count = 0;
		m_capacity = 0;
	}

	//Clear the Array and Ptr Memory
	void ClearPtr() {
		for (int i = 0; i < Size(); ++i) {
			delete m_data[i];
		}
		Clear();
	}

//operation
public:
	bool operator == (const TArray<T> & arr) {
		return this->Equal(arr);
	}

	bool operator != (const TArray<T> & arr) {
		return !this->Equal(arr);
	}

private:
	void Resize();

private:
	T * m_data;
	int m_count;
	int m_capacity;
};

template<typename T> void TArray<T>::Add(const T & element)
{
	TAssert(m_count <= m_capacity);

	//vector is full, need resize
	if (m_count == m_capacity) {
		Resize();
	}
	m_data[m_count++] = element;
}

template<typename T> void TArray<T>::Pop()
{
	TAssert(m_count > 0);
	--m_count;
}

template<typename T> void TArray<T>::Insert(const T & element, int index)
{
	TAssert(index >= 0 && index <= m_count);

	//vector is full, need resize
	if (m_count == m_capacity) {
		Resize();
	}

	for (int i = m_count; i > index; --i) {
		m_data[i] = m_data[i - 1];
	}
	m_data[index] = element;
	m_count++;
}

template<typename T> void TArray<T>::Insert(const T & element,  const T * pos)
{
	TAssert(pos >= m_data && pos < m_data + m_count);

	Insert(element, pos - m_data);
}

template<typename T> void TArray<T>::Remove(int index)
{
	TAssert(index >= 0 && index < m_count);

	for (int i = index; i < m_count-1; ++i) {
		m_data[i] = m_data[i + 1];
	}
	m_count--;
}

template<typename T> void TArray<T>::Remove(const T * pos)
{
	TAssert(pos >= m_data && pos < m_data + m_count);

	Remove((int)(pos - m_data));
}

template<typename T> void TArray<T>::RemoveAtOnce(int index)
{
	TAssert(index >= 0 && index < m_count);

	m_data[index] = m_data[m_count - 1];
	m_count--;
}

template<typename T> void TArray<T>::Resize()
{
	//init the array
	if (m_count == 0) {
		m_data = new T[ARRAY_INIT_SIZE];
		m_count = 0;
		m_capacity = ARRAY_INIT_SIZE;
	}
	//enlarge the array to double size
	else {
		int new_cap = m_capacity * 2;
		T * new_data = new T[new_cap];
		memcpy(new_data, m_data, m_capacity*sizeof(T));
		delete[] m_data;
		m_data = new_data;
		m_capacity = new_cap;
	}
}

template<typename T> void TArray<T>::Reserve(int count)
{
	if (m_count != 0) return;

	m_data = new T[count];
	m_capacity = count;
}

template<typename T> void TArray<T>::Shrink(int count)
{
	if (m_count == 0 || count >= m_count) return;

	m_data = new T[count];
	m_capacity = count;
}

template<typename T> int TArray<T>::Find(const T & element)
{
	for (int i = 0; i < m_count; ++i) {
		if (m_data[i] == element) {
			return i;
		}
	}
	return ARRAY_INDEX_NONE;
}

template<typename T> bool TArray<T>::Equal(const TArray<T> & arr)
{
	if (m_count != arr.Size()) return false;
	if (m_capacity != arr.Capacity()) return false;

	for (int i = 0; i < m_count; ++i) {
		if (m_data[i] != arr.GetData()[i]) return false;
	}
	return true;
}
