#pragma once

#include <Util/TTypes.h>
#include <stdio.h>


//
//PCK package file implementation
//
class TPackageFile
{
public:
	enum
	{
		//TBD: if need change this?
		MAX_RW_SIZE = 1024 * 1024,
	};

	TPackageFile();
	~TPackageFile();

	bool Open(const char * fileName, const char * mode);
	bool Phase2Open(int64 offset);
	bool Close();

	size_t Read(void *buffer, size_t size, size_t count);
	size_t Write(const void *buffer, size_t size, size_t count);

	size_t ReadFile(void* buffer, const size_t num_byte, FILE* stream);
	size_t WriteFile(const void* buffer, const size_t num_byte, FILE* stream);

	void Seek(int64 offset, int origin);

	int64 Tell();
	inline int64 GetPackageFileSize() { return m_size1 + m_size2; }
	void SetPackageFileSize(int64 fileSize);

private:
	char		m_path[MAX_PATH];
	char		m_path2[MAX_PATH];
	char		m_mode[32];

	FILE *		m_pFile1;
	FILE *		m_pFile2;

	int64		m_size1;
	int64		m_size2;

	int64		m_filePos;
};

