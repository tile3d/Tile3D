#include "TFile.h"
#include "TFileDir.h"
#include <Container/TString.h>
#include <string.h>
#include <Util/TAssert.h>

TFile::TFile()
{
	m_pFile = nullptr;
	m_fullPathFileName[0] = '\0';
	m_relativeFileName[0] = '\0';
	m_flags = 0;
	m_timestamp = 0;
	m_hasOpen = false;
}

TFile::~TFile()
{
	Close();
}


void TFile::Close()
{
	if (m_pFile) {
		fclose(m_pFile);
		m_pFile = nullptr;
	}
	m_hasOpen = false;
}

bool TFile::Open(const char* folderName, const char* fileName, int flags)
{
	char fullPathFileName[MAX_PATH];
	TFileDir::GetInstance()->GetFullPath(fullPathFileName, folderName, fileName);

	return Open(fullPathFileName, flags);
}

bool TFile::Open(const char* fullPath, int flags)
{
	// If already opened, we must first close it!
	if (m_hasOpen) {
		Close();
	}

	// Get a relative path name of this file, may use a little time, but
	// this call is not too often, so this is not matter
	TFileDir::GetInstance()->GetRelativePath(fullPath, m_relativeFileName);

	char szOpenFlag[32];
	szOpenFlag[0] = '\0';
	if (flags & TFILE_OPENEXIST)
		strcat(szOpenFlag, "r");

	if (flags & TFILE_CREATENEW)
		strcat(szOpenFlag, "w");

	if (flags & TFILE_OPENAPPEND)
		strcat(szOpenFlag, "a");

	//If there is no binary or text flag, the default is binary;
	if (flags & TFILE_TEXT)
		strcat(szOpenFlag, "t");
	else
		strcat(szOpenFlag, "b");

	m_pFile = fopen(m_fullPathFileName, szOpenFlag);
	if (NULL == m_pFile)
		return false;

	int magic;
	if (m_flags & TFILE_CREATENEW)	//	Create new file
	{
		m_flags = flags;
		if (m_flags & TFILE_TEXT)
		{
			magic = 0x54584f4d;
			if (!(m_flags & TFILE_NOHEAD))
				fwrite(&magic, 4, 1, m_pFile);
		}
		else
		{
			magic = 0x42584f4d;
			if (!(m_flags & TFILE_NOHEAD))
				fwrite(&magic, 4, 1, m_pFile);
		}
	}
	else	//	Open a normal file
	{
		m_flags = flags & (~(TFILE_BINARY | TFILE_TEXT));

		fread(&magic, 4, 1, m_pFile);
		if (magic == 0x42584f4d)
			m_flags |= TFILE_BINARY;
		else if (magic == 0x54584f4d)
			m_flags |= TFILE_TEXT;
		else
		{
			//Default we use text mode, for we can edit it by hand, and we will not add 
			//the shitting FOURCC at the beginning of the file
			m_flags |= TFILE_TEXT;
			fseek(m_pFile, 0, SEEK_SET);
		}
	}

	m_timestamp = TFileDir::GetInstance()->GetFileTimeStamp(m_fullPathFileName);
	m_hasOpen = true;
	return true;
}

bool TFile::OpenWithAbsFullPath(const char* fullPath, int flags)
{
	// If already opened, we must first close it!
	if (m_hasOpen)
		Close();

	char szOpenFlag[32];

	szOpenFlag[0] = '\0';
	if (flags & TFILE_OPENEXIST)
		strcat(szOpenFlag, "r");

	if (flags & TFILE_CREATENEW)
		strcat(szOpenFlag, "w");

	if (flags & TFILE_OPENAPPEND)
		strcat(szOpenFlag, "a");

	//If there is no binary or text flag, the default is binary;
	if (flags & TFILE_TEXT)
		strcat(szOpenFlag, "t");
	else
		strcat(szOpenFlag, "b");


	m_pFile = fopen((const char*)fullPath, szOpenFlag);
	if (NULL == m_pFile)
	{
		return false;
	}

	strncpy(m_fullPathFileName, fullPath, MAX_PATH);
	int dwFOURCC;

	if (flags & TFILE_CREATENEW)	//	Create new file
	{
		m_flags = flags;
		if (m_flags & TFILE_TEXT)
		{
			dwFOURCC = 0x54584f4d;
			if (!(m_flags & TFILE_NOHEAD))
				fwrite(&dwFOURCC, 4, 1, m_pFile);
		}
		else
		{
			dwFOURCC = 0x42584f4d;
			if (!(m_flags & TFILE_NOHEAD))
				fwrite(&dwFOURCC, 4, 1, m_pFile);
		}
	}
	else	//	Open a normal file
	{
		m_flags = flags & (~(TFILE_BINARY | TFILE_TEXT));

		fread(&dwFOURCC, 4, 1, m_pFile);
		if (dwFOURCC == 0x42584f4d)
			m_flags |= TFILE_BINARY;
		else if (dwFOURCC == 0x54584f4d)
			m_flags |= TFILE_TEXT;
		else
		{
			//Default we use text mode, for we can edit it by hand, and we will not add
			//the shitting FOURCC at the beginning of the file
			m_flags |= TFILE_TEXT;
			fseek(m_pFile, 0, SEEK_SET);
		}
	}

	//	int idFile = _fileno(m_pFile);
	//	struct _stat fileStat;
	//	_fstat(idFile, &fileStat);
	//	m_dwTimeStamp = (int)fileStat.st_mtime;

	m_timestamp = TFileDir::GetInstance()->GetFileTimeStamp(fullPath);
	m_hasOpen = true;
	return true;
}

bool TFile::Read(void* pBuffer, int bufferLength, int * pReadLength)
{
	*pReadLength = (int)fread(pBuffer, 1, bufferLength, m_pFile);
	return true;
}

bool TFile::Write(const void* pBuffer, int bufferLength, int * pWriteLength)
{
	*pWriteLength = (int)fwrite(pBuffer, 1, bufferLength, m_pFile);
	return true;
}

bool TFile::ReadLine(char * szLineBuffer, int bufferLength, int * pReadLength)
{
	if (!fgets(szLineBuffer, bufferLength, m_pFile))
		return false;

	//chop the \n\r
	if (szLineBuffer[0] && (szLineBuffer[strlen(szLineBuffer) - 1] == '\n' || szLineBuffer[strlen(szLineBuffer) - 1] == '\r'))
		szLineBuffer[strlen(szLineBuffer) - 1] = '\0';

	if (szLineBuffer[0] && (szLineBuffer[strlen(szLineBuffer) - 1] == '\n' || szLineBuffer[strlen(szLineBuffer) - 1] == '\r'))
		szLineBuffer[strlen(szLineBuffer) - 1] = '\0';

	*pReadLength = (int)strlen(szLineBuffer) + 1;
	return true;
}

bool TFile::ReadString(char * szLineBuffer, int bufferLength, int * pReadLength)
{
	char ch;
	int nStrLen = 0;

	fread(&ch, 1, 1, m_pFile);
	while (ch)
	{
		szLineBuffer[nStrLen] = ch;
		nStrLen++;

		if (nStrLen >= bufferLength)
			return false;

		fread(&ch, 1, 1, m_pFile);
	}

	szLineBuffer[nStrLen] = '\0';

	*pReadLength = nStrLen + 1;
	return true;
}

bool TFile::WriteString(const TString& str)
{
	//	Write length of string
	int iLen = str.GetLength();
	fwrite(&iLen, 1, sizeof(int), m_pFile);

	//	Write string data
	if (iLen)
		fwrite((const char*)str, 1, iLen, m_pFile);

	return true;
}

//TBD: check if new and delete use the memory pool
bool TFile::ReadString(TString& str)
{
	//	Read length of string
	int len;
	fread(&len, 1, sizeof(int), m_pFile);

	//	Read string data
	if (len)
	{
		char* szBuf = new char[len + 1];
		if (!szBuf)
			return false;

		fread(szBuf, 1, len, m_pFile);
		szBuf[len] = '\0';
		str = szBuf;
		delete[] szBuf;
	}
	else
		str = "";

	return true;
}

bool TFile::WriteLine(const char* szLineBuffer)
{
	if (fprintf(m_pFile, "%s\n", szLineBuffer) < 0)
		return false;
	return true;
}

int TFile::GetPos()
{
	int pos;
	pos = (int)ftell(m_pFile);
	return pos;
}

bool TFile::Seek(int offset, TFILE_SEEK origin)
{
	int start = SEEK_SET;
	switch (origin)
	{
		case TFILE_SEEK_SET:	start = SEEK_SET;	break;
		case TFILE_SEEK_CUR:	start = SEEK_CUR;	break;
		case TFILE_SEEK_END:	start = SEEK_END;	break;
		default:
		{
			TAssert(0);
			return false;
		}
	}

	if (0 != fseek(m_pFile, offset, start))
		return false;

	return true;
}

bool TFile::ResetPointer()
{
	fseek(m_pFile, 0, SEEK_SET);
	return true;
}

//	Get file length
int TFile::GetFileLength() const
{
	TAssert(m_pFile);
	int pos, end;
	pos = ftell(m_pFile);
	fseek(m_pFile, 0, SEEK_END);
	end = ftell(m_pFile);
	fseek(m_pFile, pos, SEEK_SET);
	return end;
}

bool TFile::Flush()
{
	fflush(m_pFile);
	return true;
}



