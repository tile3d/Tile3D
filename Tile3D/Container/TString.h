#pragma once

#include <string.h>
#include <stdlib.h>
#include <Common/TAssert.h>

//
//TBD
//1) AllocBuffer(adjust the alloc size to match the memory pool)
//2) Consider the benefit of reference count (cons and pro)
//3) multipile thread problem (refcount lock problem)
//4) move ctor
class TString
{
public:
	struct TStringData
	{
		int m_refcount;		// reference count
		int m_len;			// length of data (not including terminator)
		int m_maxlen;		// maximum length of data (not including terminator)

		char* Data() { return (char*)(this + 1); }
	};

	struct TStringDouble
	{
		unsigned char m_doublebits[sizeof(double)];
	};

//ctor and dtor
public:
	TString() : m_pStr(m_pEmptyStr) {}
	TString(const TString & str);
	TString(const char * str);
	TString(const char * str, int len);
	TString(char ch, int count);
	~TString();


//operation
public:
	const TString& operator = (char ch);
	const TString& operator = (const char* str);
	const TString& operator = (const TString& str);

	const TString& operator += (char ch);
	const TString& operator += (const char* str);
	const TString& operator += (const TString& str);

	friend TString operator + (const TString& str1, const TString& str2) { return TString(str1, str2); }
	friend TString operator + (char ch, const TString& str) { return TString(ch, str); }
	friend TString operator + (const TString& str, char ch) { return TString(str, ch); }
	friend TString operator + (const char* str1, const TString& str2) { return TString(str1, str2); }
	friend TString operator + (const TString& str1, const char* str2) { return TString(str1, str2); }

	int Compare(const char* szStr) const;
	int CompareNoCase(const char* szStr) const;

	bool operator == (char ch) const { return (GetLength() == 1 && m_pStr[0] == ch); }
	bool operator == (const char* szStr) const;
	bool operator == (const TString& str) const;

	bool operator != (char ch) const { return !(GetLength() == 1 && m_pStr[0] == ch); }
	bool operator != (const char* szStr) const { return !((*this) == szStr); }
	bool operator != (const TString& str) const { return !((*this) == str); }

	bool operator > (const char* szStr) const { return Compare(szStr) > 0; }
	bool operator < (const char* szStr) const { return Compare(szStr) < 0; }
	bool operator >= (const char* szStr) const { return !((*this) < szStr); }
	bool operator <= (const char* szStr) const { return !((*this) > szStr); }

	char operator [] (int n) const { TAssert(n >= 0 && n <= GetLength()); return m_pStr[n]; }
	char& operator [] (int n);

	operator const char* () const { return m_pStr; }

private:
	//	These constructors are used to optimize operations such as 'operator +'
	TString(const TString& str1, const TString& str2);
	TString(char ch, const TString& str);
	TString(const TString& str, char ch);
	TString(const char* str1, const TString& str2);
	TString(const TString& str1, const char* str2);

//method
public:
	// if the String is empty
	bool IsEmpty() const { return m_pStr == m_pEmptyStr;}

	//	Empty string
	void Empty() { FreeBuffer(GetData()); m_pStr = m_pEmptyStr; }

	int GetLength() const { return GetData()->m_len; }

	//	Convert to upper case
	void ToUpper();

	//	Convert to lower case
	void ToLower();

	void Assign(unsigned char * buf, unsigned int buf_len);

	//	Format string
	TString& Format(const char* szFormat, ...);

	//	Get Buffer
	char* GetBuffer(int iMinSize);
	//	Release buffer
	void ReleaseBuffer(int iNewSize);

	//	Trim left
	void TrimLeft();
	void TrimLeft(char ch);
	void TrimLeft(const char* szChars);
	//	Trim right
	void TrimRight();
	void TrimRight(char ch);
	void TrimRight(const char* szChars);

	//	Cut left sub string
	void CutLeft(int n);
	//	Cut right sub string
	void CutRight(int n);

	//	Finds a character or substring inside a larger string. 
	int Find(char ch, int iStart) const;
	int Find(const char* szSub, int iStart) const;
	//	Finds a character inside a larger string; starts from the end. 
	int ReverseFind(char ch) const;
	//	Finds the first matching character from a set. 
	int FindOneOf(const char* szCharSet) const;

	//	Get left string
	TString Left(int n) const {
		int iLen = GetLength();
		return TString(m_pStr, iLen < n ? iLen : n);
	}

	//	Get right string
	TString Right(int n) const {
		int iFrom = GetLength() - n;
		return Mid(iFrom < 0 ? 0 : iFrom, n);
	}

	//	Get Mid string
	TString Mid(int iFrom, int iNum = -1) const {
		int iLen = GetLength() - iFrom;
		if (iLen <= 0 || !iNum)
			return TString();	//	Return empty string

		if (iNum > 0 && iLen > iNum)
			iLen = iNum;

		return TString(m_pStr + iFrom, iLen);
	}

	//	Convert string to integer
	int ToInt() const {
		return IsEmpty() ? 0 : atoi(m_pStr);
	}

	//	Convert string to float
	float ToFloat() const {
		return IsEmpty() ? 0.0f : (float)atof(m_pStr);
	}

	//	Convert string to __int64
	long long ToInt64() const {
		return IsEmpty() ? 0 : atoll(m_pStr);
	}

	//	Convert string to double
	double ToDouble() const {
		return IsEmpty() ? 0.0 : atof(m_pStr);
	}

	char * ToString() const {
		return m_pStr;
	}

	TString& Replace(const char* szFrom, const char* szTo);	
	TString& Replace(const char cFrom, const char cTo);

private:
	//get the string 
	TStringData* GetData() const { return (TStringData*)m_pStr - 1; }

	//add the reference count
	void AddRef() const {
		GetData()->m_refcount++;
	}

	//dec the reference count
	void DecRef() const {
		GetData()->m_refcount--;
	}

	//Alocate string buffer (iLen: length of data (not including terminator))
	char* AllocBuffer(int iLen);

	//free the string buffer
	void FreeBuffer(TStringData * pStrData);

	//	Safed strlen()
	int SafeStrLen(const char* szStr) { return szStr ? static_cast<int>(strlen(szStr)) : 0; }

	//	Allocate memory and copy string
	 char* AllocThenCopy(const char* str, int len);

	 char* AllocThenCopy(const char* szSrc, char ch, int iLen);

	 char* AllocThenCopy(char ch, const char* szSrc, int iLen);

	 char* AllocThenCopy(const char* s1, const char* s2, int iLen1, int iLen2);

	 void StringCopy(char* szDest, const char* szSrc, int iLen);

	 bool StringEqual(const char* s1, const char* s2, int iLen) const;

private:
	char*	m_pStr;
	static char * m_pEmptyStr;
};

