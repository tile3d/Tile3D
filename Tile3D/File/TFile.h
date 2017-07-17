#pragma once

#include <stdio.h>
#include <Util/TTypes.h>
#include <Core/TObject.h>

class TString;
class TFile : public TObject
{
public:
	enum TFILE_FLAG 
	{
		TFILE_OPENEXIST				= 0x00000001,
		TFILE_CREATENEW				= 0x00000002,
		TFILE_OPENAPPEND			= 0x00000004,
		TFILE_TEXT					= 0x00000008,
		TFILE_BINARY				= 0x00000010,
		TFILE_NOHEAD				= 0x00000020,
		TFILE_TEMPMEMORY			= 0x00000040,	//	Use temporary memory alloctor, used by AFileImage,
		//	AFILE_TEMPMEMORY is default flag now except AFILE_NOTEMPMEMORY
		//	is specified.
		TFILE_NOTEMPMEMORY			= 0x00000080	//	Don't use temporary memory alloctor, used by AFileImage, 
		//	this flag is excluding with AFILE_TEMPMEMORY
	};

	enum TFILE_SEEK
	{
		TFILE_SEEK_SET = SEEK_SET,
		TFILE_SEEK_CUR = SEEK_CUR,
		TFILE_SEEK_END = SEEK_END,
	};

public:
	TFile();
	virtual ~TFile();

	virtual bool Open(const char* folderName, const char* fileName, int flags);
	virtual bool Open(const char* szFullPath, int dwFlags);
	virtual bool OpenWithAbsFullPath(const char* fullPath, int dwFlags);

	virtual bool Read(void* pBuffer, int bufferLength, int * pReadLength);
	virtual bool Write(const void* pBuffer, int dwBufferLength, int * pWriteLength);

	virtual bool ReadLine(char * szLineBuffer, int bufferLength, int * pReadLength);
	virtual bool ReadString(char * szLineBuffer, int bufferLength, int * pReadLength);
	virtual bool WriteString(const TString& str);
	virtual bool ReadString(TString& str);
	virtual bool WriteLine(const char* szLineBuffer);

	virtual int GetPos();
	virtual bool Seek(int iOffset, TFILE_SEEK origin);
	virtual int GetFileLength() const;
	
	bool Flush();
	virtual bool ResetPointer();

	int GetTimeStamp() const { return m_timestamp; }
	int GetFlags() const { return m_flags; }
	
	const char * GetFileName() const { return m_fullPathFileName; }
	const char * GetRelativeFileName() const { return m_relativeFileName; }

	bool IsText() { return (m_flags & TFILE_TEXT) ? true : false; }
	bool IsBinary() { return !IsText(); }

private:
	void Close();

protected:
	FILE * m_pFile;
	char m_fullPathFileName[MAX_PATH];		//fullpath file name
	char m_relativeFileName[MAX_PATH];		//relative file name
	int m_flags;
	int m_timestamp;
	bool m_hasOpen;
};

