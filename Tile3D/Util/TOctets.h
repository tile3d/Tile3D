#pragma once


class TOctets
{
public:
	explicit TOctets(int n);

	TOctets(const TOctets & rhs) {

	}

	int Size() { return m_count; }

	char* Begin() { return m_data; }
	char* End() { return m_data + m_count; }

	bool Empty() { return m_count == 0; }

	void Clear() {
		m_count = 0;
	}

	char * operator[](int pos) {

	}
private:
	char * m_data;
	int m_count;
	int m_capacity;
};

