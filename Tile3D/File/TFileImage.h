#pragma once

#include "TFile.h"
#include "TPackageFile.h"
#include <Container/TString.h>

class TFileImage : public TFile
{
public:
	TFileImage();
	virtual ~TFileImage();

	virtual bool Open(const char* fullPath, int flags);
	virtual bool Open(const char* folderName, const char* fileName, int flags);
	virtual bool OpenWithAbsFullPath(const char* fullPath, int flags);
	virtual bool Close();

	virtual bool Read(void* pBuffer, int bufferLength, int * pReadLength);
	virtual bool Write(const void* pBuffer, int bufferLength, int * pWriteLength);

	virtual bool ReadLine(char * lineBuffer, int dwBufferLength, int * pdwReadLength);
	virtual bool ReadString(char * lineBuffer, int dwBufferLength, int * pdwReadLength);
	virtual bool WriteLine(const char* lineBuffer);
	virtual bool WriteString(const TString& str);
	virtual bool ReadString(TString& str);

	virtual int GetPos();
	virtual bool Seek(int offset, TFILE_SEEK origin);
	virtual bool ResetPointer();
	//	Get file length
	virtual int GetFileLength() const { return m_fileLength; }

	inline unsigned char* GetFileBuffer() { return m_pFileImage; }

	bool ReadImage(unsigned char* pBuffer, int nSize, int * pReadSize); // read some size of data into a buffer;
	bool ReadImageByLine(char * szLineBuffer, int nMaxLength, int * pReadSize); // read a line into a buffer;
	bool SeekImage(int nOffset, int startPos); // offset current pointer

protected:
	bool Init(const char* szFullPath, bool bTempMem);
	bool Release();
	bool ReadFileData(const char* szFullPath, bool bPrintError = true);
	bool ReadPackData(AFilePackBase* pPackage, bool bTempMem, bool bPrintError = true);
	bool ReadAPKFileData(const char* szRelativePath, bool bPrintError = true);

private:
	AFilePackBase *	m_pPackage;		//	package object this file image open with
	unsigned char*	m_pFileImage;	//	Memory pointer of the file image in memory;
	int				m_nCurPtr;		//	In index into the file image buffer;
	int				m_fileLength;	//	File length;
	int				m_handle;		//	Handle in file package
	bool			m_bTempMem;		//	true, use temporary memory alloctor
};



