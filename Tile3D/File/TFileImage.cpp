#include "TFileImage.h"
#include "TFileDir.h"
#include "TPackage.h"
#include "TPackageMan.h"
#include <Common/TLog.h>
#include <Core/TMemory.h>
#include <Sys/TSysFile.h>

TFileImage::TFileImage() : TFile()
{
	m_pPackage = nullptr;
	m_pFileImage = nullptr;
	m_nCurPtr = 0;
	m_fileLength = 0;
	m_handle = 0;
}

TFileImage::~TFileImage()
{
	Close();
}

bool TFileImage::Init(const char * fullName)
{
	strncpy(m_fullPathFileName, fullName, MAX_PATH);

	TFileDir::GetInstance()->GetRelativePath(fullName, m_relativeFileName);
	m_pPackage = TPackageMan::GetInstance()->GetPackage(m_relativeFileName);

	if (m_pPackage)
	{
		// using package file first
		unsigned long fileLen;

		//	Init from a package
		if (!(m_handle = m_pPackage->OpenSharedFile(m_relativeFileName, &m_pFileImage, &fileLen)))
		{
			// can't find it in package, so see if can load from disk.
			m_pPackage = NULL;
			//TBD: justify if we can load the file from disk directly
			return InitWithSepFile(fullName);
		}

		m_fileLength = (int)fileLen;
		m_timestamp = 0;
		return true;
	}
	else {
		if (!InitWithSepFile(fullName)){
			TLog::Log(LOG_ERR, "FILE", "TFileImage::Init(): Can not open file [%s] from disk!", fullName);
			return false;
		}
	}
	return true;
}

bool TFileImage::InitWithSepFile(const char * fullName)
{
	// no package for this file, so try to load from disk.
	FILE* pFile;
	if (!(pFile = fopen(fullName, "rb")))
	{
		TLog::Log(LOG_ERR, "FILE", "TFileImage::InitWithSepFile(): Can not open file [%s] from disk!", fullName);
		return false;
	}

	fseek(pFile, 0, SEEK_END);
	//TBD: if the file is above 2G, need change the method
	m_fileLength = ftell(pFile);
	if (m_fileLength == 0)
	{
		TLog::Log(LOG_ERR, "FILE", "TFileImage::InitWithSepFile(): Init The file [%s] is zero length!", fullName);
		return false;
	}

	fseek(pFile, 0, SEEK_SET);

	if (!(m_pFileImage = new unsigned char[m_fileLength]))
	{
		TLog::Log(LOG_ERR, "FILE", "TFileImage::InitWithSepFile():Init Not enough memory! FileName : %s, FileLength : %d", fullName, m_fileLength);
		return false;
	}
	fread(m_pFileImage, m_fileLength, 1, pFile);

	m_timestamp = TSysFile::GetFileTimeStamp(fullName);
	fclose(pFile);
	return true;
}

bool TFileImage::Open(const char* folderName, const char * fileName, int dwFlags)
{
	char fullPath[MAX_PATH];
	TFileDir::GetInstance()->GetFullPath(fullPath, folderName, fileName);
	return Open(fullPath, dwFlags);
}

bool TFileImage::Open(const char* szFullPath, int flags)
{
	// If we have opened it already, we must close it;
	if (m_hasOpen)
		Close();


	if (!Init(szFullPath))
	{
		return false;
	}

	if ((flags & TFILE_OPENEXIST) == 0)
	{
		TLog::Log(LOG_ERR, "FILE", "TFileImage::Open():Open Current we only support read flag to operate a file image");
		return false;
	}

	int fileMagic;
	int nRead;

	m_flags = flags;
	m_flags = flags & (~(TFILE_BINARY | TFILE_TEXT));
	if (!ReadImage((unsigned char*)&fileMagic, 4, &nRead))
		return false;

	if (fileMagic == 0x42584f4d)
		m_flags |= TFILE_BINARY;
	else if (fileMagic == 0x54584f4d)
		m_flags |= TFILE_TEXT;
	else
	{
		//Default we use text mode, for we can edit it by hand, and we will not add 
		//the shitting FOURCC at the beginning of the file
		m_flags |= TFILE_TEXT;
		SeekImage(0, TFILE_SEEK_SET);
	}

	m_hasOpen = true;
	return true;
}

bool TFileImage::Close()
{
	Release();

	m_nCurPtr = 0;
	m_fullPathFileName[0] = '\0';
	return true;
}

bool TFileImage::Release()
{
	if (!m_pPackage)
	{
		if (m_pFileImage)
		{
			delete[] m_pFileImage;
			m_pFileImage = nullptr;
		}
	}
	else
	{
		if (m_handle && m_pFileImage)
		{
			m_pPackage->CloseSharedFile(m_handle);
			m_pFileImage = NULL;
			m_handle = 0;
		}
	}

	return true;
}

bool TFileImage::ResetPointer()
{
	SeekImage(0, TFILE_SEEK_SET);
	return true;
}

bool TFileImage::ReadImage(unsigned char* pBuffer, int nSize, int * pReadSize)
{
	int nSizeToRead = nSize;

	if (m_nCurPtr + nSizeToRead > m_fileLength)
		nSizeToRead = m_fileLength - m_nCurPtr;

	if (nSizeToRead <= 0)
	{
		*pReadSize = 0;
		return nSize == 0 ? true : false;
	}

	memcpy(pBuffer, m_pFileImage + m_nCurPtr, nSizeToRead);
	m_nCurPtr += nSizeToRead;
	*pReadSize = nSizeToRead;
	return true;
}

bool TFileImage::ReadImageByLine(char * lineBuffer, int maxLength, int * pReadSize)
{
	int nSizeRead = 0;

	memset(lineBuffer, 0, maxLength);
	while (m_nCurPtr < m_fileLength)
	{
		unsigned char byteThis = m_pFileImage[m_nCurPtr];

		if (byteThis != 0x0d && byteThis != 0x0a)
		{
			// Not \n or \r, so copy it into the buffer;
			lineBuffer[nSizeRead++] = m_pFileImage[m_nCurPtr++];
		}
		else
		{
			// We also need to copy \n into the buffer;
			lineBuffer[nSizeRead++] = m_pFileImage[m_nCurPtr++];
			lineBuffer[nSizeRead] = '\0';
			if (byteThis == 0x0d)
			{
				// We need to check if next byte is \r, if so, just remove it;
				if (m_nCurPtr < m_fileLength)
				{
					if (m_pFileImage[m_nCurPtr] == 0x0a)
					{
						m_nCurPtr++;
						nSizeRead++;
					}
				}
			}

			break;
		}
	}

	*pReadSize = nSizeRead;

	if (nSizeRead <= 0)
		return false;
	return true;
}

bool TFileImage::SeekImage(int offset, int startPos)
{
	switch (startPos)
	{
	case TFILE_SEEK_SET:
		m_nCurPtr = offset;
		break;
	case TFILE_SEEK_CUR:
		m_nCurPtr += offset;
		break;
	case TFILE_SEEK_END:
		m_nCurPtr = m_fileLength + offset;
		break;
	default:
		return false;
	}
	if (m_nCurPtr < 0)
		m_nCurPtr = 0;
	else if (m_nCurPtr > m_fileLength) // To be compatible with fseek, we have to let the file pointer beyond the last character;
		m_nCurPtr = m_fileLength;
	return true;
}

bool TFileImage::ReadLine(char * lineBuffer, int bufferLength, int * pReadLength)
{
	int nReadSize;

	if (!ReadImageByLine(lineBuffer, bufferLength, &nReadSize))
		return false;

	//chop the \n\r
	if (lineBuffer[0] && (lineBuffer[strlen(lineBuffer) - 1] == '\n' || lineBuffer[strlen(lineBuffer) - 1] == '\r'))
		lineBuffer[strlen(lineBuffer) - 1] = '\0';

	if (lineBuffer[0] && (lineBuffer[strlen(lineBuffer) - 1] == '\n' || lineBuffer[strlen(lineBuffer) - 1] == '\r'))
		lineBuffer[strlen(lineBuffer) - 1] = '\0';

	*pReadLength = strlen(lineBuffer) + 1;
	return true;
}

bool TFileImage::Read(void* pBuffer, int bufferLength, int * pReadLength)
{
	int nReadSize;
	if (!ReadImage((unsigned char*)pBuffer, bufferLength, &nReadSize))
		return false;

	*pReadLength = nReadSize;
	return true;
}

bool TFileImage::Write(const void* pBuffer, int bufferLength, int * pWriteLength)
{
	return false;
}

int TFileImage::GetPos()
{
	return m_nCurPtr;
}

bool TFileImage::Seek(int offset, TFILE_SEEK origin)
{
	return SeekImage(offset, (int)origin);
}

bool TFileImage::WriteLine(const char* lineBuffer)
{
	return false;
}

bool TFileImage::ReadString(char * lineBuffer, int bufferLength, int * pReadLength)
{
	char ch;
	int readLength;
	int nStrLen = 0;

	Read(&ch, 1, &readLength);
	while (ch)
	{
		lineBuffer[nStrLen] = ch;
		nStrLen++;

		if (nStrLen >= bufferLength)
			return false;

		Read(&ch, 1, &readLength);
	}

	lineBuffer[nStrLen] = '\0';

	*pReadLength = nStrLen + 1;
	return true;
}

bool TFileImage::WriteString(const TString& str)
{
	return false;
}

bool TFileImage::ReadString(TString& str)
{
	//	Only binary file is supported
	/*	if (m_dwFlags & TFILE_TEXT)
	{
		ASSERT(!(m_dwFlags & TFILE_TEXT));
		return false;
	}
	*/

	int readSize;
	int length;
	if (!ReadImage((unsigned char*)&length, sizeof(int), &readSize))
		return false;

	//	Read string data
	if (length)
	{
		char* buf = new char[length + 1];
		if (!buf)
			return false;

		if (!ReadImage((unsigned char*)buf, length, &readSize))
		{
			delete[] buf;
			return false;
		}
		buf[length] = '\0';
		str = buf;

		delete[] buf;
	}
	else
		str = "";

	return true;
}

