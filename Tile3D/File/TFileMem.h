#pragma once

#include "TFile.h"

class TFileMem : public TFile
{
public:
	TFileMem();
	~TFileMem();

	bool Attach(unsigned char* pBuf, int dwBufLen, int dwGrowBy);
	unsigned char* Detach();
	bool ExtendFileBeforeWrite(int writeSize);

	bool Read(void* pBuffer, int bufferLength, int* pReadLength);
	bool Write(const void* pBuffer, int bufferLength, int* pWriteLength);
	
	bool ReadLine(char* lineBuffer, int bufferLength, int* pReadLength);
	bool ReadString(char* lineBuffer, int bufferLength, int* pReadLength);
	bool WriteLine(const char* lineBuffer);
	bool WriteString(const TString& str);
	bool ReadString(TString& str);

	bool SetFileLength(int fileLen);

	bool Seek(int offset, TFILE_SEEK origin);


protected:
	int m_bufLen;
	int m_fileLen;
	unsigned char * m_pBuf;
	int m_offset;
	int m_growBy;
};
