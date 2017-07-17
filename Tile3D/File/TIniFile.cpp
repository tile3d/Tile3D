#include "TIniFile.h"
#include "TFile.h"
#include <Util/TLog.h>
#include <Util/TAssert.h>

TIniFile::TIniFile()
{
	m_bOpened = false;
	m_pCurSect = NULL;
}

TIniFile::~TIniFile()
{
	if (m_bOpened)
		TIniFile::Close();
}

// Close file
// TBD: need check if have memory leak
void TIniFile::Close()
{
	//	Release all sections and keys
	for (int i = 0; i < m_sects.Size(); i++)
	{
		TIniFileSection* pSect = m_sects[i];

		for (int j = 0; j < pSect->m_keys.Size(); j++) {
			delete pSect->m_keys[j];
		}

		delete pSect;
	}
	m_bOpened = false;
}

/*	Open ini file

Return true for success, otherwise return false.

szFile: ini file's name, can be both absolute path and relative path.
*/
bool TIniFile::Open(const char* szFile)
{
	TFileImage fi;
	if (!fi.Open("", szFile, TFILE_OPENEXIST | TFILE_BINARY | TFILE_TEMPMEMORY))
	{
		fi.Close();
		TLog::Log(LOG_ERR, "AIniFile::Open Can't open file [%s].", szFile);
		return false;
	}

	if (!Open(&fi))
	{
		fi.Close();
		return false;
	}

	fi.Close();

	return true;
}

/*	Open ini file

Return true for success, otherwise return false.

pFile: AFile object which has been opened using binary mode.
*/
bool TIniFile::Open(TFile* pFile)
{
	TAssert(pFile);

	if (m_bOpened)
	{
		TAssert(0);
		TLog::Log(LOG_ERR, "FILE", "AIniFile::Open, ini file couldn't be opened twice");
		return false;
	}

	int fileLen = pFile->GetFileLength();
	if (fileLen <= 0)
	{
		TLog::Log(LOG_ERR, "FILE", "AIniFile::Open, Empty ini file");
		return true;
	}

	//	Load all file into memory
	unsigned char * pBuf = new (unsigned char*)[fileLen];
	if (!pBuf)
	{
		TLog::Log(LOG_ERR, "FILE", "AIniFile::Open, Not enough memory");
		return true;
	}

	int dwRead;
	if (!pFile->Read(pBuf, fileLen, &dwRead) || dwRead != fileLen)
	{
		delete [] pBuf;
		TLog::Log(LOG_ERR, "FILE", "AIniFile::Open, Failed to read file content");
		return true;
	}

	m_pCurSect = NULL;

	if (!ParseFile(pBuf, pBuf + fileLen)) {
		TLog::Log(LOG_ERR, "FILE", "AIniFile::Open, Failed to parse ini file");
	}

	delete[] pBuf;
	m_bOpened = true;
	return true;
}

/*	Parse file.

Return true for success, otherwise return false.

pBuf: file buffer.
pEnd: file buffer ending position
*/
bool TIniFile::ParseFile(unsigned char* pBuf, unsigned char* pEnd)
{
	unsigned char* pLine = pBuf;
	int iLineLen;

	while (true)
	{
		if (!(iLineLen = GetLineLength(pLine, pEnd)))
			break;

		ParseLine(pLine, pLine + iLineLen);
		pLine += iLineLen;
	}
	return true;
}


/*	Get line length

Return line's length in unsigned chars

pLine: line buffer.
pEnd: file buffer ending position
*/
int TIniFile::GetLineLength(unsigned char* pLine, unsigned char* pEnd)
{
	unsigned char* pCur = pLine;
	int iLen = 0;

	while (pCur < pEnd && *pCur != '\n')
	{
		iLen++;
		pCur++;
	}

	if (pCur < pEnd && *pCur == '\n')
		iLen++;

	return iLen;
}

/*	Add a section

Return section's address for success, otherwise return NULL

bComment: true, comment section
strName: section's name
*/
TIniFile::TIniFileSection* TIniFile::AddSection(bool comment, const TString& name)
{
	TIniFileSection* pSection = new TIniFileSection;
	if (!pSection)
		return NULL;

	pSection->m_comment = comment;
	pSection->m_name = name;

	m_sects.Add(pSection);

	return pSection;
}

/*	Create a key

Return key's address for success, otherwise return NULL.

strKey: key's name
*/
TIniFile::TIniFileKey* TIniFile::CreateKey(const TString& key)
{
	TIniFileKey* pKey = new TIniFileKey;
	if (!pKey)
		return NULL;

	pKey->m_key = key;
	return pKey;
}

/*	Parse a line.

pLine: line buffer.
pEnd: line buffer ending position
*/
void TIniFile::ParseLine(unsigned char* pLine, unsigned char* pEnd)
{
	unsigned char* pCur = pLine;

	//	Search for the first character which large than 32
	while (pCur < pEnd && *pCur <= 32)
		pCur++;

	//	Empty line ?
	if (pCur >= pEnd)
		return;

	//	Comment line ?
	if (*pCur == ';')
	{
		unsigned char* pBegin = pCur;
		while (pCur < pEnd && *pCur != '\n')
			pCur++;

		//	Get comment content		
		TString name((const char*)pBegin, pCur - pBegin);
		name.TrimRight();

		//	Create section
		AddSection(true, name);
		return;
	}

	//	New section ?
	if (*pCur == '[')
	{
		unsigned char* pBegin = ++pCur;
		while (pCur < pEnd && *pCur != ']')
			pCur++;

		//	[] doesn't match ?
		if (pCur >= pEnd)
			return;

		//	Check section name
		TString name((const char*)pBegin, pCur - pBegin);
		name.TrimRight();

		//	Create section
		m_pCurSect = AddSection(false, name);
		return;
	}

	//	Parse key-values ...
	if (!m_pCurSect)
		return;

	unsigned char* pBegin = pCur;
	while (pCur < pEnd && *pCur != '=')
		pCur++;

	//	Didn't find '=' ?
	if (*pCur != '=')
		return;

	//	Get key name
	TString name((const char*)pBegin, pCur - pBegin);
	name.TrimRight();

	TIniFileKey* pKey = CreateKey(name);
	TAssert(pKey);

	pKey->m_key = name;

	if (!ParseValue(pKey, pCur + 1, pEnd))
		delete pKey;
	else
		m_pCurSect->m_keys.Add(pKey);
}

/*	Parse key value

Return true for success, otherwise return false.

pLine: line buffer.
pEnd: line buffer ending position
*/
bool TIniFile::ParseValue(TIniFileKey* pKey, unsigned char* pBuf, unsigned char* pEnd)
{
	unsigned char * pCur = pBuf;

	//	Search for the first character which large than 32
	while (pCur < pEnd && (*pCur <= 32 || *pCur == ','))
		pCur++;

	if (pCur >= pEnd)
		return false;

	pKey->m_value = TString((const char*)pCur, pEnd - pCur);
	pKey->m_value.TrimRight();
	return true;
}

/*	Get values as integer

Return integer value.

szSect: section name
szKey: key name
iDefault: default value will be used if specified value was failed to be found
*/
int TIniFile::GetValueAsInt(const char* szSect, const char* szKey, int iDefault)
{
	if (!m_bOpened)
		return iDefault;

	TString* pstrVal = SearchValue(szSect, szKey);
	if (pstrVal)
		return pstrVal->ToInt();

	return iDefault;
}

/*	Get values as string

Return integer value.

szSect: section name
szKey: key name
iDefault: default value will be used if specified value was failed to be found
*/
TString TIniFile::GetValueAsString(const char* szSect, const char* szKey,
	const char* szDefault/* NULL */)
{
	if (!m_bOpened)
		return TString(szDefault);

	TString* pstrVal = SearchValue(szSect, szKey);
	if (pstrVal)
		return *pstrVal;

	return TString(szDefault);
}

/*	Get values as string

Return integer value.

szSect: section name
szKey: key name
iDefault: default value will be used if specified value was failed to be found
*/
float TIniFile::GetValueAsFloat(const char* szSect, const char* szKey, float fDefault)
{
	if (!m_bOpened)
		return fDefault;

	TString* pstrVal = SearchValue(szSect, szKey);
	if (pstrVal)
		return pstrVal->ToFloat();

	return fDefault;
}

/*	Get values as integer array

Return true for success, otherwise return false.

szSect: section name
szKey: key name
iNumInt: number of integer
pBuf: array buffer
*/
bool TIniFile::GetValueAsIntArray(const char* szSect, const char* szKey, int iNumInt, int* pBuf)
{
	if (!m_bOpened)
		return false;

	TString* pstrVal = SearchValue(szSect, szKey);
	if (!pstrVal)
		return false;

	const char* pCur = *pstrVal;
	const char* pEnd = pCur + pstrVal->GetLength();

	for (int i = 0; i < iNumInt; i++)
	{
		//	Search for the first character which large than 32
		while (pCur < pEnd && (*pCur <= 32 || *pCur == ','))
			pCur++;

		if (pCur >= pEnd)
			return false;

		const char* pBegin = pCur;
		while (pCur < pEnd && *pCur > 32 && *pCur != ',')
			pCur++;

		TString strValue((const char*)pBegin, pCur - pBegin);
		pBuf[i] = strValue.ToInt();
	}

	return true;
}

bool TIniFile::GetValueAsFloatArray(const char* szSect, const char* szKey, int iNumFloat, float* pBuf)
{
	if (!m_bOpened)
		return false;

	TString* pstrVal = SearchValue(szSect, szKey);
	if (!pstrVal)
		return false;

	const char* pCur = *pstrVal;
	const char* pEnd = pCur + pstrVal->GetLength();

	for (int i = 0; i < iNumFloat; i++)
	{
		//	Search for the first character which large than 32
		while (pCur < pEnd && (*pCur <= 32 || *pCur == ','))
			pCur++;

		if (pCur >= pEnd)
			return false;

		const char* pBegin = pCur;
		while (pCur < pEnd && *pCur > 32 && *pCur != ',')
			pCur++;

		TString strValue((const char*)pBegin, pCur - pBegin);
		pBuf[i] = strValue.ToFloat();
	}

	return true;
}

bool TIniFile::GetValueAsIntRect(const char* szSect, const char* szKey, ARectI* pRect)
{
	int aVals[4];
	if (!GetValueAsIntArray(szSect, szKey, 4, aVals))
		return false;

	pRect->SetRect(aVals[0], aVals[1], aVals[2], aVals[3]);
	return true;
}

/*	Write integer value

Return true for success, otherwise return false.

szSect: section name
szKey: key name
iValue: value will be written.
*/
bool TIniFile::WriteIntValue(const char* szSect, const char* szKey, int iValue)
{
	TIniFileKey* pKey = GetKey(szSect, szKey);
	if (!pKey)
		return false;

	pKey->m_value.Format("%d", iValue);

	return true;
}

/*	Write integer value

Return true for success, otherwise return false.

szSect: section name
szKey: key name
szValue: value will be written.
*/
bool TIniFile::WriteStringValue(const char* szSect, const char* szKey, const char* szValue)
{
	TIniFileKey* pKey = GetKey(szSect, szKey);
	if (!pKey)
		return false;

	pKey->m_value = szValue;

	return true;
}

/*	Write integer value

Return true for success, otherwise return false.

szSect: section name
szKey: key name
fValue: value will be written.
*/
bool TIniFile::WriteFloatValue(const char* szSect, const char* szKey, float fValue)
{
	TIniFileKey* pKey = GetKey(szSect, szKey);
	if (!pKey)
		return false;

	pKey->m_value.Format("%f", fValue);
	return true;
}

/*	Search specified value string

Return string's address for success, otherwise return NULL.

szSect: section name
szKey: key name
*/
TString* TIniFile::SearchValue(const char* szSect, const char* szKey)
{
	TAssert(szSect && szSect[0]);
	TAssert(szKey && szKey[0]);

	int i, j;

	for (i = 0; i < m_sects.Size(); i++)
	{
		TIniFileSection* pSect = m_sects[i];

		if (pSect->m_comment || pSect->m_name.CompareNoCase(szSect))
			continue;

		for (j = 0; j < pSect->m_keys.Size(); j++)
		{
			TIniFileKey* pKey = pSect->m_keys[j];
			if (pKey->m_key.CompareNoCase(szKey))
				continue;

			return &pKey->m_value;
		}
	}

	return NULL;
}

/*	Search specified section

Return section's address for success, otherwise return NULL.

szSect: section's name
*/
TIniFile::TIniFileSection* TIniFile::SearchSection(const char* szSect)
{
	for (int i = 0; i < m_sects.Size(); i++)
	{
		TIniFileSection* pSection = m_sects[i];

		if (!pSection->m_comment && !pSection->m_name.CompareNoCase(szSect))
			return pSection;
	}

	return NULL;
}

/*	Search key in specified section

Return key's address for success, otherwise return NULL.

pSection: section in key expected to exist.
szKey: key's name
*/
TIniFile::TIniFileKey* TIniFile::SearchKey(TIniFileSection* pSection, const char* szKey)
{
	for (int i = 0; i < pSection->m_keys.Size(); i++)
	{
		TIniFileKey* pKey = pSection->m_keys[i];
		if (!pKey->m_key.CompareNoCase(szKey))
			return pKey;
	}

	return NULL;
}

/*	Get specified key, create if it doesn't exist

Return key's address for success, otherwise return false.

szSect: section name
szKey: key name
*/
TIniFile::TIniFileKey* TIniFile::GetKey(const char* szSect, const char* szKey)
{
	TAssert(szSect && szSect[0]);
	TAssert(szKey && szKey[0]);

	TIniFileSection* pSection = SearchSection(szSect);
	if (!pSection)
	{
		//	Create a new section
		pSection = AddSection(false, szSect);
	}

	TIniFileKey* pKey = SearchKey(pSection, szKey);
	if (!pKey)
	{
		//	Create a new key
		pKey = CreateKey(szKey);
		pSection->m_keys.Add(pKey);
	}

	return pKey;
}

//	Save modified data
bool TIniFile::Save(const char* szFile)
{
	FILE* fp = fopen(szFile, "w+");
	if (!fp)
	{
		TLog::Log(LOG_ERR, "FILE", "TIniFile::Save, Failed to create flie %s", szFile);
		return false;
	}

	int i, j;
	TString strValue;
	for (i = 0; i < m_sects.Size(); i++)
	{
		TIniFileSection* pSect = m_sects[i];
		if (pSect->m_comment)
		{
			fprintf(fp, "%s\n", pSect->m_name);
			continue;
		}

		//	Write section name
		strValue = "[" + pSect->m_name + "]";
		fprintf(fp, "%s\n", strValue);

		for (j = 0; j < pSect->m_keys.Size(); j++)
		{
			TIniFileKey* pKey = pSect->m_keys[j];

			//	Write key name and value
			strValue = pKey->m_key + " = " + pKey->m_value;
			fprintf(fp, "%s\n", strValue);
		}

		fprintf(fp, "\n");
	}

	fclose(fp);
	return true;
}

bool TIniFile::Save(TFile* pFile)
{
	int i, j;
	TString strValue;
	for (i = 0; i < m_sects.Size(); i++)
	{
		TIniFileSection* pSect = m_sects[i];
		if (pSect->m_comment)
		{
			pFile->WriteLine(pSect->m_name);
			continue;
		}

		//	Write section name
		strValue = "[" + pSect->m_name + "]";
		pFile->WriteLine(strValue);

		for (j = 0; j < pSect->m_keys.Size(); j++)
		{
			TIniFileKey* pKey = pSect->m_keys[j];

			//	Write key name and value
			strValue = pKey->m_key + " = " + pKey->m_value;
			pFile->WriteLine(strValue);
		}

		pFile->WriteLine("");
	}

	return true;
}
