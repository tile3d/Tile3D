#include "TWScriptFile.h"
#include <string.h>
#include "TFile.h"
#include "TFileImage.h"
#include <Common/TLog.h>
#include <Core/TMemory.h>

TWScriptFile::TWScriptFile()
{
	memset(&m_script, 0, sizeof(m_script));
}

TWScriptFile::~TWScriptFile()
{
	TWScriptFile::Close();
}

/*	Open an existing script file and load it into memory

Return true for success, otherwise return false

pFile: script file's pointer. In order to be sure that script file operations
are always right, this file should be opened as binary file !!!
*/
bool TWScriptFile::Open(TFile *pFile)
{
	int fileLen = pFile->GetFileLength();
	if (!fileLen)
	{
		TLog::Log(LOG_ERR, "FILE", "TWScriptFile::Open, Empty ini file");
		return true;
	}

	char* pBuf = new char[fileLen];
	if (!pBuf)
	{
		TLog::Log(LOG_ERR, "FILE", "TWScriptFile::Open, Not enough memory");
		return false;
	}

	//	Read whole file into memory
	int readNum;
	if (!pFile->Read(pBuf, fileLen, &readNum) || readNum != fileLen)
	{
		delete[] pBuf;
		TLog::Log(LOG_ERR, "FILE", "TWScriptFile::Open, Failed to read file content");
		return false;
	}

	m_script.pFileBuf = pBuf;
	m_script.pStart = (wchar_t*)pBuf + 1;		//skip the unicode magic number
	m_script.pCur = m_script.pStart;
	m_script.pEnd = (wchar_t*)(pBuf + fileLen);
	m_script.iLine = 0;
	return true;
}

bool TWScriptFile::Open(const char* szFile)
{
	TFileImage fileImage;

	if (!fileImage.Open("", szFile, TFile::TFILE_OPENEXIST | TFile::TFILE_BINARY | TFile::TFILE_TEMPMEMORY))
	{
		fileImage.Close();
		return false;
	}

	if (!Open(&fileImage))
	{
		fileImage.Close();
		return false;
	}

	fileImage.Close();
	return true;
}

//	Close file
void TWScriptFile::Close()
{
	if (m_script.pFileBuf) {
		delete[] m_script.pFileBuf;
	}

	memset(&m_script, 0, sizeof(m_script));
}

//	Reset pointers so that this file can be prase again
void TWScriptFile::ResetScriptFile()
{
	m_script.pCur = m_script.pStart;
	m_script.iLine = 0;
}

/*	Get next token and move file pointer forward.

Return true for success, otherwise return false

bCrossLine: true, search next token until it is found or all buffer has been checked
false, only search next token in current line
*/
bool TWScriptFile::GetNextToken(bool bCrossLine)
{
NewLine:
	//	Search for the first character which large than 32
	while (m_script.pCur < m_script.pEnd)
	{
		if (*m_script.pCur > 32 && *m_script.pCur != ';' && *m_script.pCur != ',')
			break;

		if (*m_script.pCur++ == '\n')
		{
			if (!bCrossLine)
			{
				m_script.pCur--;	//	Let search pointer still stop in this line
				return false;
			}

			m_script.iLine++;
		}
	}

	if (m_script.pCur >= m_script.pEnd)
		return false;

	//	Skip comment lines those begin with '//'
	if (m_script.pCur[0] == '/' && m_script.pCur[1] == '/')
	{
		//	This is a note line, search it's ending.
		while (m_script.pCur < m_script.pEnd && *m_script.pCur != '\n') {
			m_script.pCur++;
		}

		if (m_script.pCur >= m_script.pEnd)	//	Found nothing
			return false;
		else if (!bCrossLine)	//	Don't search cross line
			return false;

		m_script.pCur++;	//	Skip '\n'
		m_script.iLine++;
		goto NewLine;
	}

	//	Text between /* */ are also comment
	if (m_script.pCur[0] == '/' && m_script.pCur[1] == '*')
	{
		bool bError = false;

		m_script.pCur += 2;		//	Skip /*
		while (m_script.pCur[0] != '*' || m_script.pCur[1] != '/')
		{
			if (m_script.pCur >= m_script.pEnd)		//	Found nothing
				return false;
			else if (*m_script.pCur == '\n')
			{
				if (!bCrossLine)
				{
					//	This is a fatal error, we should return false. 
					//	But we must search the '*/' so that next time our begin point
					//	isn't in comment paragraph
					bError = true;
				}

				m_script.iLine++;
			}

			m_script.pCur++;
		}

		m_script.pCur += 2;	//	Skip */
		if (bError)
			return false;

		goto NewLine;
	}

	int i = 0;

	//	Copy string in "" or () pair
	if (*m_script.pCur == '"' || *m_script.pCur == '(')
	{
		char cEnd;
		if (*m_script.pCur == '"')
			cEnd = '"';
		else
			cEnd = ')';

		//	Quoted token
		m_script.pCur++;	//	Skip " or (
		while (m_script.pCur < m_script.pEnd && *m_script.pCur != cEnd)
		{
			if (i >= MAX_LINELEN - 1)
				return false;

			m_token[i++] = *m_script.pCur++;
		}

		m_script.pCur++;	//	Skip " or )
	}
	else	//	Is a normal token
	{
		while (m_script.pCur < m_script.pEnd && *m_script.pCur > 32 &&
			*m_script.pCur != ';' && *m_script.pCur != ',')
		{
			if (i >= MAX_LINELEN - 1)
				return false;

			m_token[i++] = *m_script.pCur++;
		}
	}

	m_token[i] = '\0';
	return true;
}

/*	Peek next token and don't move file pointer

Return true for success, otherwise return false

bCrossLine: true, search next token until it is found or all buffer has been checked
false, only search next token in current line
*/
bool TWScriptFile::PeekNextToken(bool bCrossLine)
{
	//	Record current pointer and line
	wchar_t * pCur = m_script.pCur;
	int iLine = m_script.iLine;

	bool bRet = GetNextToken(bCrossLine);

	//	Restore pointer and line
	m_script.pCur = pCur;
	m_script.iLine = iLine;
	return bRet;
}

/*	Search specified token. This function get next token and check whether it match
specified string, if match, then stop and return true, otherwise get next token
again until all file is checked or token is found.

Note: This will be crossing-line search.

Return true for success, otherwise return false.

szToken: specified token will be searched
bCaseSensitive: true, case sensitive
*/
bool TWScriptFile::MatchToken(wchar_t* token, bool bCaseSensitive)
{
	while (GetNextToken(true))
	{
		if (bCaseSensitive)
		{
			if (!_wcsicmp(m_token, token))
				return true;
		}
		else
		{
			if (!_wcsicmp(m_token, token))
				return true;
		}
	}

	return false;
}

/*	Skip current line and ignore all rest content in it.

Return true for success, otherwise return false
*/
bool TWScriptFile::SkipLine()
{
	while (m_script.pCur < m_script.pEnd)
	{
		if (*m_script.pCur++ == '\n')
			break;
	}

	m_script.iLine++;
	return true;
}



//	Get next token as float
float TWScriptFile::GetNextTokenAsFloat(bool bCrossLine)
{
	GetNextToken(bCrossLine);
	float fVal;
	swscanf_s(m_token, L"%f", &fVal);
	return fVal;
}

//	Get next token as int
int	TWScriptFile::GetNextTokenAsInt(bool bCrossLine)
{
	GetNextToken(bCrossLine);
	return _wtoi(m_token);
}
