#include "TString.h"
#include <stdarg.h>
#include <stdio.h>

#pragma warning (disable: 4996)

struct TStringEmpty
{
	TString::TStringData m_data;
	char	m_str[1];

	TStringEmpty()
	{
		m_data.m_refcount = 0;
		m_data.m_len = 0;
		m_data.m_maxlen = 0;
		m_str[0] = '\0';
	}
};

//	For an empty string, m_pchData will point here
static TStringEmpty m_StrEmpty;
char* TString::m_pEmptyStr = m_StrEmpty.m_str;

TString::TString(const TString & str)
{
	if (str.IsEmpty()) {
		m_pStr = m_pEmptyStr;
		return;
	}

	str.AddRef();
	m_pStr = str.m_pStr;
}

TString::TString(const char* str)
{
	int len = SafeStrLen(str);
	m_pStr = AllocThenCopy(str, len);
}

TString::TString(const char* str, int len)
{
	m_pStr = AllocThenCopy(str, len);
}


TString::TString(char ch, int count)
{
	m_pStr = AllocBuffer(count);
	memset(m_pStr, ch, count);
	m_pStr[count] = '\0';
}

TString::~TString()
{
	TStringData * pData = GetData();
	
	FreeBuffer(pData);
	m_pStr = m_pEmptyStr;
}

const TString& TString::operator = (char ch)
{
	if (!ch)
	{
		Empty();
		return *this;
	}

	TStringData * pData = GetData();

	if (IsEmpty())
		m_pStr = AllocBuffer(1);
	else if (pData->m_refcount  > 1)
	{
		pData->m_refcount--;
		m_pStr = AllocBuffer(1);
	}
	else
		pData->m_refcount = 1;

	m_pStr[0] = ch;
	m_pStr[1] = '\0';

	GetData()->m_len = 1;
	return *this;
}

const TString& TString::operator = (const char* str)
{
	int len = SafeStrLen(str);
	if (len == 0)
	{
		Empty();
		return *this;
	}

	TStringData* pData = GetData();

	if (pData->m_refcount > 1)
	{
		pData->m_refcount--;
		m_pStr = AllocThenCopy(str, len);
	}
	else
	{
		if (len <= pData->m_maxlen)
		{
			StringCopy(m_pStr, str, len);
			pData->m_len = len;
		}
		else
		{
			FreeBuffer(pData);
			m_pStr = AllocThenCopy(str, len);
		}
	}

	return *this;
}


const TString& TString::operator = (const TString& str)
{
	if (m_pStr == str.m_pStr)
		return *this;

	if (str.IsEmpty())
	{
		Empty();
		return *this;
	}

	TStringData* pSrcData = str.GetData();
	FreeBuffer(GetData());
	pSrcData->m_refcount++;
	m_pStr = str.m_pStr;
	return *this;
}


const TString& TString::operator += (char ch)
{
	if (!ch)
		return *this;

	TStringData* pData = GetData();

	if (pData->m_refcount > 1)
	{
		pData->m_refcount--;
		m_pStr = AllocThenCopy(m_pStr, ch, pData->m_len + 1);
		return *this;
	}

	int len = pData->m_len + 1;
	if (len <= pData->m_maxlen)
	{
		m_pStr[len - 1] = ch;
		m_pStr[len] = '\0';
		pData->m_len++;
	}
	else
	{
		m_pStr = AllocThenCopy(m_pStr, ch, len);
		FreeBuffer(pData);
	}

	return *this;
}

const TString& TString::operator += (const char* str)
{
	int iLen2 = SafeStrLen(str);
	if (!iLen2)
		return *this;

	TStringData* pData = GetData();

	if (pData->m_refcount > 1)
	{
		pData->m_refcount--;
		m_pStr = AllocThenCopy(m_pStr, str, pData->m_len, iLen2);
		return *this;
	}

	int iLen = pData->m_len + iLen2;
	if (iLen <= pData->m_len)
	{
		StringCopy(m_pStr + pData->m_len, str, iLen2);
		pData->m_len = iLen;
	}
	else
	{
		m_pStr = AllocThenCopy(m_pStr, str, pData->m_len, iLen2);
		FreeBuffer(pData);
	}

	return *this;
}

const TString& TString::operator += (const TString& str)
{
	int iLen2 = str.GetLength();
	if (!iLen2)
		return *this;

	TStringData * pData = GetData();

	if (pData->m_refcount > 1)
	{
		pData->m_refcount--;
		m_pStr = AllocThenCopy(m_pStr, str.m_pStr, pData->m_len, iLen2);
		return *this;
	}

	int iLen = pData->m_len + iLen2;
	if (iLen <= pData->m_len)
	{
		StringCopy(m_pStr + pData->m_len, str.m_pStr, iLen2);
		pData->m_len = iLen;
	}
	else
	{
		m_pStr = AllocThenCopy(m_pStr, str.m_pStr, pData->m_len, iLen2);
		FreeBuffer(pData);
	}

	return *this;
}

//	Convert to upper case
void TString::ToUpper()
{
	int iLen = GetLength();
	if (!iLen)
		return;

	TStringData* pData = GetData();
	if (pData->m_refcount > 1)
	{
		pData->m_refcount--;
		m_pStr = AllocThenCopy(m_pStr, iLen);
	}

	//tbd: need change to cross platform
	strupr(m_pStr);
}

//	Convert to lower case
void TString::ToLower()
{
	int iLen = GetLength();
	if (!iLen)
		return;

	TStringData* pData = GetData();
	if (pData->m_refcount > 1)
	{
		pData->m_refcount--;
		m_pStr = AllocThenCopy(m_pStr, iLen);
	}

	//tbd: need change to cross platform
	strlwr(m_pStr);
}

void TString::Assign(unsigned char * buf, unsigned int buf_len)
{
	m_pStr = AllocThenCopy((char*)buf, (int)buf_len);
}

//	Format string
TString& TString::Format(const char* szFormat, ...)
{
	va_list argList;
	va_start(argList, szFormat);

	int iNumWritten;
	char buffer[1024];
	iNumWritten = vsprintf(buffer, szFormat, argList);

	TStringData * pData = GetData();

	if (pData->m_refcount > 1)
		pData->m_refcount--;
	else if (iNumWritten <= pData->m_maxlen)
	{
		vsprintf(m_pStr, szFormat, argList);
		pData->m_len = SafeStrLen(m_pStr);
		goto End;
	}
	else {	//	iMaxLen > pData->iMaxLen
		FreeBuffer(pData);
	}

	m_pStr = AllocBuffer(iNumWritten+1);
	strncpy(m_pStr, buffer, iNumWritten + 1);

	GetData()->m_len = SafeStrLen(m_pStr);

End:
	va_end(argList);
	return *this;
}

/*	Get buffer. If you have changed content buffer returned by GetBuffer(), you
must call ReleaseBuffer() later. Otherwise, ReleaseBuffer() isn't necessary.

Return buffer's address for success, otherwise return NULL.

iMinSize: number of bytes in string buffer user can changed.
*/
char* TString::GetBuffer(int iMinSize)
{
	if (iMinSize < 0)
	{
		TAssert(iMinSize >= 0);
		return NULL;
	}

	//	Ensure we won't allocate an empty string when iMinSize == 1
	if (!iMinSize)
		iMinSize = 1;

	TStringData* pData = GetData();

	if (IsEmpty())
	{
		m_pStr = AllocBuffer(iMinSize);
		m_pStr[0] = '\0';
		GetData()->m_len = 0;
	}
	else if (pData->m_refcount > 1)
	{
		pData->m_refcount--;

		if (iMinSize <= pData->m_len)
		{
			m_pStr = AllocThenCopy(m_pStr, pData->m_len);
		}
		else
		{
			char* szOld = m_pStr;
			m_pStr = AllocBuffer(iMinSize);
			StringCopy(m_pStr, szOld, pData->m_len);
			GetData()->m_len = pData->m_len;
		}
	}
	else if (iMinSize > pData->m_maxlen)
	{
		char* szOld = m_pStr;
		m_pStr = AllocBuffer(iMinSize);
		StringCopy(m_pStr, szOld, pData->m_len);
		GetData()->m_len = pData->m_len;
		FreeBuffer(pData);
	}
	return m_pStr;
}

/*	If you have changed content of buffer returned by GetBuffer(), you must call
ReleaseBuffer() later. Otherwise, ReleaseBuffer() isn't necessary.

iNewSize: new size in bytes of string. -1 means string is zero ended and it's
length can be got by strlen().
*/
void TString::ReleaseBuffer(int iNewSize)
{
	TStringData* pData = GetData();
	if (pData->m_refcount != 1)
	{
		TAssert(pData->m_refcount == 1);	//	Ensure GetBuffer has been called.
		return;
	}

	if (iNewSize == -1)
		iNewSize = SafeStrLen(m_pStr);

	if (iNewSize > pData->m_maxlen)
	{
		TAssert(iNewSize <= pData->m_maxlen);
		return;
	}

	if (iNewSize == 0)
	{
		Empty();
	}
	else
	{
		pData->m_len = iNewSize;
		m_pStr[iNewSize] = '\0';
	}
}

//	Trim left
void TString::TrimLeft()
{
	if (!GetLength())
		return;

	int i;
	unsigned char * aStr = (unsigned char *)m_pStr;
	for (i = 0; aStr[i]; i++)
	{
		if (aStr[i] > 32)
			break;
	}

	CutLeft(i);
}

//	Trim left
void TString::TrimLeft(char ch)
{
	if (!GetLength())
		return;

	int i;

	for (i = 0; m_pStr[i]; i++)
	{
		if (m_pStr[i] != ch)
			break;
	}

	CutLeft(i);
}

//	Trim left
void TString::TrimLeft(const char* szChars)
{
	if (!GetLength())
		return;

	int i, j;

	for (i = 0; m_pStr[i]; i++)
	{
		for (j = 0; szChars[j]; j++)
		{
			if (m_pStr[i] == szChars[j])
				break;
		}

		if (!szChars[j])
			break;
	}

	CutLeft(i);
}

//	Trim right
void TString::TrimRight()
{
	if (!GetLength())
		return;

	int i, iLen = GetLength();
	unsigned char * aStr = (unsigned char *)m_pStr;

	for (i = iLen - 1; i >= 0; i--)
	{
		if (aStr[i] > 32)
			break;
	}

	CutRight(iLen - 1 - i);
}

//	Trim right
void TString::TrimRight(char ch)
{
	if (!GetLength())
		return;

	int i, iLen = GetLength();

	for (i = iLen - 1; i >= 0; i--)
	{
		if (m_pStr[i] != ch)
			break;
	}

	CutRight(iLen - 1 - i);
}

//	Trim right
void TString::TrimRight(const char* szChars)
{
	if (!GetLength())
		return;

	int i, j, iLen = GetLength();

	for (i = iLen - 1; i >= 0; i--)
	{
		for (j = 0; szChars[j]; j++)
		{
			if (m_pStr[i] == szChars[j])
				break;
		}

		if (!szChars[j])
			break;
	}

	CutRight(iLen - 1 - i);
}

//	Cut left sub string
void TString::CutLeft(int n)
{
	if (!GetLength() || n <= 0)
		return;

	TStringData* pData = GetData();

	if (n >= pData->m_len)
	{
		Empty();
		return;
	}

	int iNewLen = pData->m_len - n;

	if (pData->m_refcount > 1)
	{
		pData->m_refcount--;
		m_pStr = AllocThenCopy(m_pStr + n, iNewLen);
		return;
	}

	for (int i = 0; i < iNewLen; i++)
		m_pStr[i] = m_pStr[n + i];

	m_pStr[iNewLen] = '\0';
	pData->m_len = iNewLen;
}

//	Cut right sub string
void TString::CutRight(int n)
{
	if (!GetLength() || n <= 0)
		return;

	TStringData * pData = GetData();

	if (n >= pData->m_len)
	{
		Empty();
		return;
	}

	int iNewLen = pData->m_len - n;

	if (pData->m_refcount > 1)
	{
		pData->m_refcount--;
		m_pStr = AllocThenCopy(m_pStr, iNewLen);
		return;
	}

	m_pStr[iNewLen] = '\0';
	pData->m_len = iNewLen;
}

//	Finds a character inside a larger string. 
//	Return -1 for failure.
int TString::Find(char ch, int iStart) const
{
	int iLen = GetLength();
	if (!iLen || iStart < 0 || iStart >= iLen)
		return -1;

	for (int i = iStart; i < iLen; i++)
	{
		if (m_pStr[i] == ch)
			return i;
	}

	return -1;
}


//	Finds a substring inside a larger string. 
//	Return -1 for failure.
int TString::Find(const char* szSub, int iStart) const
{
	int iLen = GetLength();
	if (!iLen || iStart < 0 || iStart >= iLen)
		return -1;

	char* pTemp = strstr(m_pStr + iStart, szSub);
	if (!pTemp)
		return -1;

	return pTemp - m_pStr;
}

//	Finds a character inside a larger string; starts from the end. 
//	Return -1 for failure.
int TString::ReverseFind(char ch) const
{
	if (!GetLength())
		return -1;

	char* pTemp = strrchr(m_pStr, ch);
	if (!pTemp)
		return -1;

	return pTemp - m_pStr;
}

//	Finds the first matching character from a set. 
//	Return -1 for failure.
int TString::FindOneOf(const char* szCharSet) const
{
	int iLen = GetLength();
	if (!iLen)
		return -1;

	return ((int)strcspn(m_pStr, szCharSet) == iLen) ? -1 : 0;
}


TString::TString(const TString& str1, const TString& str2)
{
	m_pStr = AllocThenCopy(str1.m_pStr, str2.m_pStr, str1.GetLength(), str2.GetLength());
}

TString::TString(char ch, const TString& str)
{
	m_pStr = AllocThenCopy(ch, str.m_pStr, str.GetLength() + 1);
}

TString::TString(const TString& str, char ch)
{
	m_pStr = AllocThenCopy(str.m_pStr, ch, str.GetLength() + 1);
}

TString::TString(const char* szStr, const TString& str)
{
	m_pStr = AllocThenCopy(szStr, str.m_pStr, SafeStrLen(szStr), str.GetLength());
}

TString::TString(const TString& str, const char* szStr)
{
	m_pStr = AllocThenCopy(str.m_pStr, szStr, str.GetLength(), SafeStrLen(szStr));
}

//	Alocate string buffer
char* TString::AllocBuffer(int iLen)
{
	TStringData* pData;
	char* pBuf;

	if (iLen < 64)
	{
		pBuf = new char[64 + sizeof(TStringData)];
		pData = (TStringData*)pBuf;

		pData->m_refcount = 1;
		pData->m_len = iLen;
		pData->m_maxlen = 63;
	}
	else if (iLen < 128)
	{
		pBuf = new char[128 + sizeof(TStringData)];
		pData = (TStringData*)pBuf;

		pData->m_refcount = 1;
		pData->m_len = iLen;
		pData->m_maxlen = 127;
	}
	else if (iLen < 256)
	{
		pBuf = new char[256 + sizeof(TStringData)];
		pData = (TStringData*)pBuf;

		pData->m_refcount = 1;
		pData->m_len = iLen;
		pData->m_maxlen = 255;
	}
	else if (iLen < 512)
	{
		pBuf = new char[512 + sizeof(TStringData)];
		pData = (TStringData*)pBuf;

		pData->m_refcount = 1;
		pData->m_len = iLen;
		pData->m_maxlen = 511;
	}
	else
	{
		pBuf = new char[iLen + 1 + sizeof(TStringData)];
		pData = (TStringData*)pBuf;

		pData->m_refcount = 1;
		pData->m_len = iLen;
		pData->m_maxlen = iLen;
	}

	return pBuf + sizeof(TStringData);
}

//	Free string data buffer
void TString::FreeBuffer(TStringData * pStrData)
{
	switch (pStrData->m_refcount)
	{
	case 0:	return;
	case 1:

		delete[] pStrData;
		break;

	default:
		pStrData->m_refcount--;
		break;
	}
}

/*
need handle the reference count
TString& TString::Replace(const char* szFrom, const char* szTo)
{
	TString* Result = (TString*)this;
	int fromlen = (int)strlen(szFrom);
	int i = Result->Find(szFrom, 0);
	while (i != -1)
	{
		*Result = Result->Left(i) + TString(szTo) + Result->Mid(i + fromlen);
		i = Result->Find(szFrom, 0);
	}

	return *this;
}

TString& TString::Replace(const char cFrom, const char cTo)
{
	TString* Result = (TString*)this;
	int iLen = GetLength();
	if (!iLen)
		return *this;

	for (int i = 0; i < iLen; i++)
	{
		if (m_pStr[i] == cFrom)
			m_pStr[i] = cTo;
	}

	return *this;
}
*/

int TString::Compare(const char* szStr) const
{
	if (m_pStr == szStr)
		return 0;

	return strcmp(m_pStr, szStr);
}

int TString::CompareNoCase(const char* szStr) const
{
	if (m_pStr == szStr)
		return 0;

	//TBD: change to Sys function
	return _stricmp(m_pStr, szStr);
}

bool TString::operator == (const char* szStr) const
{
	//	Note: szStr's boundary may be crossed when StringEqual() do
	//		  read operation, if szStr is shorter than 'this'. Now, this
	//		  read operation won't cause problem, but in the future,
	//		  should we check the length of szStr at first, and put the 
	//		  shorter one between 'this' and szStr front when we call StringEqual ?
	int iLen = GetLength();
	return StringEqual(m_pStr, szStr, iLen + 1);
}


//	Allocate memory and copy string
char* TString::AllocThenCopy(const char* str, int len)
{
	if (len == 0)
		return m_pEmptyStr;

	char* s = AllocBuffer(len);
	StringCopy(s, str, len);
	return s;
}

//	Allocate a new string which is merged by szSrc + ch
char* TString::AllocThenCopy(const char* szSrc, char ch, int iLen)
{
	if (!ch)
		return AllocThenCopy(szSrc, iLen - 1);

	char* s = AllocBuffer(iLen);
	StringCopy(s, szSrc, iLen - 1);

	s[iLen - 1] = ch;
	s[iLen] = '\0';

	return s;
}

//	Allocate a new string which is merged by ch + szSrc
char* TString::AllocThenCopy(char ch, const char* szSrc, int iLen)
{
	if (!ch)
		return m_StrEmpty.m_str;

	char* s = AllocBuffer(iLen);

	s[0] = ch;
	StringCopy(s + 1, szSrc, iLen - 1);

	return s;
}


//	Allocate a new string which is merged by s1 + s2
char* TString::AllocThenCopy(const char* s1, const char* s2, int iLen1, int iLen2)
{
	if (!iLen2)
		return AllocThenCopy(s1, iLen1);

	int iLen = iLen1 + iLen2;
	char* s = AllocBuffer(iLen);

	StringCopy(s, s1, iLen1);
	StringCopy(s + iLen1, s2, iLen2);

	return s;
}


//	Copy iLen characters from szSrc to szDest and add terminator at the tail of szDest
void TString::StringCopy(char* szDest, const char* szSrc, int iLen)
{
	int i, iSpan = sizeof(long);
	const long *p1 = (const long*)szSrc;
	long *p2 = (long*)szDest;

	for (i = 0; i < iLen / iSpan; i++, p1++, p2++)
		*p2 = *p1;

	for (i *= iSpan; i < iLen; i++)
		szDest[i] = szSrc[i];

	szDest[i] = '\0';
}

//	Judge whether two strings are equal
bool TString::StringEqual(const char* s1, const char* s2, int iLen) const
{
	int i, iSpan = sizeof(long);
	const long *p1 = (const long *)s1;
	const long *p2 = (const long *)s2;

	for (i = 0; i < iLen / iSpan; i++, p1++, p2++)
	{
		if (*p1 != *p2)
			return false;
	}

	for (i *= iSpan; i < iLen; i++)
	{
		if (s1[i] != s2[i])
			return false;
	}

	return true;
}

bool TString::operator == (const TString& str) const
{
	if (m_pStr == str.m_pStr)
		return true;

	int iLen = GetLength();
	if (iLen != str.GetLength())
		return false;

	return StringEqual(m_pStr, str.m_pStr, iLen);
}

char& TString::operator [] (int n)
{
	TAssert(n >= 0 && n <= GetLength());

	TStringData* pData = GetData();
	if (pData->m_refcount > 1)
	{
		pData->m_refcount--;
		m_pStr = AllocThenCopy(m_pStr, GetLength());
	}

	return m_pStr[n];
}


