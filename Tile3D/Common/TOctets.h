#pragma once

#include "TAssert.h"
#include <string.h>

class TOctets
{
public:
	TOctets() {
		m_data = nullptr;
		m_count = 0;
		m_capacity = 0;
	}

	explicit TOctets(int n) {
		Reserve(n);
	}

	TOctets(const TOctets & rhs) : m_data(nullptr), m_count(0), m_capacity(0){
		Push(rhs.Begin(), rhs.Size());
	}


	//move ctor
	TOctets(TOctets && rhs) {
		m_data = rhs.m_data;
		m_count = rhs.m_count;
		m_capacity = rhs.m_capacity;
		rhs.m_data = nullptr;
	}

	//deep copy
	TOctets & operator= (const TOctets & rhs) {
		if (this == &rhs) return *this;

		m_count = rhs.m_count;
		m_capacity = rhs.m_capacity;
		m_data = new unsigned char[m_capacity];
		memcpy(m_data, rhs.m_data, m_count);
		return *this;
	}

	TOctets(const void * buf, unsigned int buf_len) : m_data(nullptr), m_count(0), m_capacity(0)	{
		Push(buf, buf_len);
	}

	~TOctets() {
		delete[] m_data;
		m_count = 0;
		m_capacity = 0;
	}

	bool operator==(const TOctets & rhs) {
		return m_count == rhs.m_count && !memcpy(m_data, rhs.m_data, m_count);
	}

public:
	void Reserve(unsigned int size) {
		if (size > m_capacity) {
			int origSize = m_capacity;
			if (m_capacity < 16) m_capacity = 16;
			while (m_capacity < size) {
				m_capacity = m_capacity * 2;
			}

			unsigned char * new_data = new unsigned char[m_capacity];
			memcpy(new_data, m_data, m_count);
			delete[] m_data;

			m_data = new_data;
		}
	}

	void Push(unsigned char ch) {
		Reserve(m_count + 1);
		unsigned char* end = End();
		*end++ = ch;
		m_count++;
	}

	void Push(const void * buf, unsigned int buf_size) {
		Reserve(m_count + buf_size);
		memcpy(m_data + m_count, buf, buf_size);
		m_count += buf_size;
	}

	void Erase(unsigned char* begin, unsigned char* end) {
		if (begin < end) {
			unsigned int offset = begin - m_data;
			unsigned int len = (end - begin);
			memcpy(begin, end, m_count-offset-len);
			m_count -= len;
		}
	}

	unsigned int Size() const { return m_count; }

	unsigned char* Begin() { return m_data; }
	unsigned char* End() { return m_data + m_count; }

	unsigned char* Begin() const { return m_data; }
	unsigned char* End() const { return m_data + m_count; }


	bool Empty() { return m_count == 0; }

	void Clear() {
		m_count = 0;
	}

	unsigned char * operator[](unsigned int pos) {
		TAssert(pos >= 0 && pos < m_count);
		return m_data+pos;
	}


private:
	unsigned char * m_data;
	unsigned int m_count;
	unsigned int m_capacity;
};

