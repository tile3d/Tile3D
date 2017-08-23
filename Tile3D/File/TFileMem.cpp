#include "TFileMem.h"
#include <Container/TString.h>
#include <Util/TAssert.h>
#include <Util/TLog.h>
#include <string.h>
#include <Core/TMemory.h>

TFileMem::TFileMem() : TFile()
{
	m_bufLen = 0;
	m_fileLen = 0;
	m_pBuf = NULL;
	m_offset = 0;
	m_growBy = 1024;
	m_flags = TFILE_CREATENEW;	//	Read and write
}

TFileMem::~TFileMem()
{
	if (m_pBuf)
	{
		delete[] m_pBuf;
	}
}

/*	Attach memory block

Return true for success, otherwise return false.

pBuf: buffer will be attached to memory file object.
bufLen: specifies the size of the buffer in bytes.
browBy: the minimum size of memory to be allocated if memory file buffer
need to grow.

Note: If growBy is zero, AMemFile will set the file length to BufLen.
This means that the data in the memory block before it was attached to
TFileMem will be used as the file. Memory files created in this manner
cannot be grown. If GrowBy is greater than zero, TFileMem will ignore
the contents of the memory block you have attached. If you attempt to
write past the end of the file or grow the file by calling the
TFileMem:SetFileLength.
*/
bool TFileMem::Attach(unsigned char* pBuf, int bufLen, int growBy)
{
	//	A block has been attached before
	if (m_pBuf)
	{
		TAssert(0 && "Memory file is attached twice!");
		return false;
	}

	if (!growBy)
	{
		//	Read-only file
		TAssert(pBuf);
		m_fileLen = bufLen;
	}
	else
	{
		//	Read-write file
		TAssert(growBy > 0);
		m_fileLen = 0;
	}

	m_growBy = growBy;
	m_bufLen = bufLen;
	m_pBuf = pBuf;
	m_offset = 0;
	return true;
}

//	Detach memory block
unsigned char* TFileMem::Detach()
{
	m_bufLen = 0;
	m_fileLen = 0;
	unsigned char* pBuf = m_pBuf;
	m_pBuf = NULL;
	return pBuf;
}

/*	Extend size of memory file buffer before write data

Return true for success, otherwise return false.

dwWriteSize: size of data will be writen
*/
bool TFileMem::ExtendFileBeforeWrite(int writeSize)
{
	if (writeSize <= 0)
		return true;

	//	Memory file buffer is length enough ?
	if (m_offset + writeSize <= m_bufLen)
		return true;	//	Needn't extend

	int appendSize = m_offset + writeSize - m_bufLen;
	if (appendSize < m_growBy)
		appendSize = m_growBy;

	//	Extend memory file buffer
	if (!m_pBuf) {
		m_pBuf = new unsigned char[m_bufLen + appendSize];
	}
	else {
		m_pBuf = new(m_pBuf) unsigned char[m_bufLen + appendSize];
	}

	if (!m_pBuf)
	{
		//	Not enough memory to extend file
		TLog::Log(LOG_ERR, "FILE", "TFileMem::ExtendFileBeforeWrite, Not enough memory");
		return false;
	}
	m_bufLen += appendSize;
	return true;
}

/*	Read data from file

Return true for success, otherwise return false.

pBuffer: buffer used to receive read data
bufferLength: Length of data wanted to be read
pReadLength (out): size data actually read
*/
bool TFileMem::Read(void* pBuffer, int bufferLength, int* pReadLength)
{
	TAssert(m_pBuf && pBuffer);

	*pReadLength = 0;

	if (m_offset >= m_fileLen)
		return false;

	if (!bufferLength)
		return true;

	int read = bufferLength;
	if (m_offset + bufferLength > m_fileLen)
		read = m_fileLen - m_offset;

	memcpy(pBuffer, m_pBuf + m_offset, read);
	m_offset += read;
	*pReadLength = read;
	return true;
}

/*	Write data to file

Return true for success, otherwise return false.

pBuffer: buffer which contains data will be written
bufferLength: Length of data wanted to be written
pWriteLength (out): size data actually written
*/
bool TFileMem::Write(const void* pBuffer, int bufferLength, int* pWriteLength)
{
	TAssert(pBuffer && m_growBy);

	*pWriteLength = 0;

	if (m_offset > m_fileLen)
		return false;

	if (!bufferLength)
		return true;

	if (!ExtendFileBeforeWrite(bufferLength))
	{
		TLog::Log(LOG_ERR, "FILE", "TFileMem::Write, Failed to allocate memory");
		return false;
	}

	//	Memory file buffer is length enough ?
	memcpy(m_pBuf + m_offset, pBuffer, bufferLength);

	if (m_offset + bufferLength > m_fileLen)
		m_fileLen = m_offset + bufferLength;

	m_offset += bufferLength;
	*pWriteLength = bufferLength;

	return true;
}

//	Set file length
bool TFileMem::SetFileLength(int fileLen)
{
	if (fileLen <= m_bufLen)
		m_fileLen = fileLen;
	else
	{
		//	Extend memory buffer
		ExtendFileBeforeWrite(fileLen - m_bufLen);
		m_fileLen = fileLen;
	}

	return false;
}


/*	Seek file pointer

Return true for success, otherwise return false.

offset: move offset
iOrigin: origin flags AFILE_SEEK_* defined in AFile.h
*/
bool TFileMem::Seek(int offset, TFILE_SEEK origin)
{
	int dwOff;

	switch (origin)
	{
	case TFILE_SEEK_SET:

		dwOff = offset < 0 ? 0 : offset;
		break;

	case TFILE_SEEK_CUR:

		if (offset < 0)
			dwOff = (int)(-offset) >= m_offset ? 0 : m_offset + offset;
		else
			dwOff = m_offset + offset;

		break;

	case TFILE_SEEK_END:

		if (offset <= 0)
			dwOff = (int)(-offset) >= m_fileLen ? 0 : m_fileLen + offset;
		else
			dwOff = m_offset + offset;

		break;

	default:
		TAssert(0);
		return false;
	}

	m_offset = dwOff > m_fileLen ? m_fileLen : dwOff;
	return true;
}

bool TFileMem::ReadLine(char* lineBuffer, int bufferLength, int* pReadLength)
{
	int dwSizeRead = 0;

	while (m_offset < m_fileLen)
	{
		unsigned char byteThis = m_pBuf[m_offset];

		if (byteThis != 0x0d && byteThis != 0x0a)
		{
			//	Not \n or \r, so copy it into the buffer;
			lineBuffer[dwSizeRead++] = m_pBuf[m_offset++];
		}
		else
		{
			m_offset++;

			if (byteThis == 0x0d)
			{
				//	We need to check if next byte is \r, if so, just remove it;
				if (m_offset < m_fileLen)
				{
					if (m_pBuf[m_offset] == 0x0a)
						m_offset++;
				}
			}

			break;
		}
	}

	lineBuffer[dwSizeRead] = '\0';

	if (!dwSizeRead)
		return true;

	*pReadLength = dwSizeRead + 1;

	return true;
}

bool TFileMem::ReadString(char* lineBuffer, int bufferLength, int* pReadLength)
{
	char ch;
	int readLength;
	int strLen = 0;

	Read(&ch, 1, &readLength);
	while (ch)
	{
		lineBuffer[strLen++] = ch;

		if (strLen >= bufferLength)
			return false;

		Read(&ch, 1, &readLength);
	}

	lineBuffer[strLen] = '\0';

	*pReadLength = strLen + 1;

	return true;
}

bool TFileMem::WriteLine(const char* lineBuffer)
{
	TAssert(m_growBy);

	int dwWrite;

	int len = (int)strlen(lineBuffer);
	if (len)
	{
		if (!Write((void*)lineBuffer, len, &dwWrite))
			return false;
	}

	//	Write a '\n'
	char ch = '\n';
	Write(&ch, sizeof(ch), &dwWrite);

	return true;
}

bool TFileMem::WriteString(const TString& str)
{
	int dwWrite;

	//	Write length of string
	int len = str.GetLength();
	Write(&len, sizeof(int), &dwWrite);

	//	Write string data
	if (len)
	{
		if (!Write((void*)(const char*)str, len, &dwWrite))
			return false;
	}

	return true;
}

bool TFileMem::ReadString(TString& str)
{
	int dwRead;

	//	Read length of string
	int len;
	Read(&len, sizeof(int), &dwRead);

	//	Read string data
	if (len)
	{
		char* szBuf = new char[len + 1];
		if (!szBuf)
			return false;

		if (!Read(szBuf, len, &dwRead))
			return false;

		szBuf[len] = '\0';
		str = szBuf;

		delete[] szBuf;
	}
	else
		str = "";

	return true;
}
