#include "TPackageFile.h"
#include "TFileSys.h"
#include <string.h>
#include <algorithm>
#include <Util/TLog.h>
#include <Util/TAssert.h>

TPackageFile::TPackageFile()
{
	m_pFile1 = NULL;
	m_pFile2 = NULL;

	m_size1 = 0;
	m_size2 = 0;

	m_filePos = 0;
}

TPackageFile::~TPackageFile()
{
}

bool TPackageFile::Open(const char * fileName, const char * mode)
{
	Close();

	m_pFile1 = fopen(fileName, mode);
	if (NULL == m_pFile1)
		return false;

	fseek(m_pFile1, 0, SEEK_END);
	m_size1 = ftell(m_pFile1);
	fseek(m_pFile1, 0, SEEK_SET);

	m_filePos = 0;
	strncpy(m_path, fileName, MAX_PATH);
	strncpy(m_mode, mode, 32);

	strcpy(m_path2, m_path);
	TFileSys::GetInstance()->ChangeFileExt(m_path2, MAX_PATH, ".pkx");
	return true;
}

bool TPackageFile::Phase2Open(int64 offset)
{
	if (offset >= MAX_FILE_PACKAGE)
	{
		// we need the second file now;
		m_pFile2 = fopen(m_path2, m_mode);
		if (NULL == m_pFile2)
		{
			if (stricmp(m_mode, "r+b") == 0 && !TFileSys::GetInstance()->IsFileExist(m_path2))
			{
				// it is the first time we access the second file	
				m_pFile2 = fopen(m_path2, "wb+");
				if (NULL == m_pFile2)
					return false;
			}
			else
				return false;
		}

		fseek(m_pFile2, 0, SEEK_END);
		m_size2 = ftell(m_pFile2);
		fseek(m_pFile2, 0, SEEK_SET);
	}

	return true;
}

bool TPackageFile::Close()
{
	if (m_pFile2)
	{
		fclose(m_pFile2);
		m_pFile2 = NULL;
	}

	if (m_pFile1)
	{
		fclose(m_pFile1);
		m_pFile1 = NULL;
	}

	m_size1 = 0;
	m_size2 = 0;
	m_filePos = 0;
	return true;
}

size_t TPackageFile::Read(void *buffer, size_t size, size_t count)
{
	size_t size_to_read = size * count;
	int64 new_pos = m_filePos + size_to_read;

	if (new_pos <= MAX_FILE_PACKAGE)
	{
		// completely in file 1
		size_t readsize = ReadFile(buffer, size_to_read, m_pFile1);
		m_filePos += readsize;

		//	if m_filePos is moved to MAX_FILE_PACKAGE after writting at it happens,
		//	next reading operation will go to case 3 other than case 2, so we should reset m_pFile2's file pointer to
		//	ensure next reading operation to start at correct position.
		if (m_filePos == MAX_FILE_PACKAGE && m_pFile2)
			fseek(m_pFile2, 0, SEEK_SET);

		return readsize;
	}
	else if (m_filePos < MAX_FILE_PACKAGE)
	{
		// partial in file1 and partial in file 2
		size_t size_to_read1 = (size_t)(MAX_FILE_PACKAGE - m_filePos);
		size_t size_to_read2 = (size_t)(size_to_read - size_to_read1);

		// read from file1
		size_t readsize = ReadFile(buffer, size_to_read1, m_pFile1);
		if (readsize != size_to_read1)
		{
			m_filePos += readsize;
			return readsize;
		}

		if (m_pFile2)
		{
			// read from file2
			fseek(m_pFile2, 0, SEEK_SET);
			readsize += ReadFile((unsigned char*)buffer + size_to_read1, size_to_read2, m_pFile2);
		}

		m_filePos += readsize;
		return readsize;
	}
	else
	{
		// completely in file 2
		size_t readsize = ReadFile(buffer, size_to_read, m_pFile2);
		m_filePos += readsize;
		return readsize;
	}

	return 0;
}

size_t TPackageFile::Write(const void *buffer, size_t size, size_t count)
{
	size_t size_to_write = size * count;
	int64 new_size = m_filePos + size_to_write;

	if (new_size <= MAX_FILE_PACKAGE)
	{
		// case 1: completely in file 1
		size_t writesize = WriteFile(buffer, size_to_write, m_pFile1);
		m_filePos += writesize;
		if (m_filePos > m_size1)
			m_size1 = m_filePos;
		return writesize;
	}
	else if (m_filePos < MAX_FILE_PACKAGE)
	{
		// case 2: partial in file1 and partial in file 2
		size_t size_to_write1 = MAX_FILE_PACKAGE - m_filePos;
		size_t size_to_write2 = size_to_write - size_to_write1;

		// write to file1
		size_t writesize1 = WriteFile(buffer, size_to_write1, m_pFile1);
		m_filePos += writesize1;
		if (m_filePos > m_size1)
			m_size1 = m_filePos;

		// By MSDN:
		// fwrite returns the number of full items actually written, which may be less than count if an error occurs. 
		// Also, if an error occurs, the file-position indicator cannot be determined.
		if (writesize1 != size_to_write1)
		{
			fseek(m_pFile1, m_filePos, SEEK_SET);
			return writesize1;
		}

		m_size1 = MAX_FILE_PACKAGE;

		if (!m_pFile2)
			Phase2Open(MAX_FILE_PACKAGE);

		// write to file2
		fseek(m_pFile2, 0, SEEK_SET);
		size_t writesize2 = WriteFile((unsigned char*)buffer + size_to_write1, size_to_write2, m_pFile2);
		m_filePos += writesize2;
		if (m_filePos > m_size1 + m_size2)
			m_size2 = m_filePos - m_size1;
		return writesize1 + writesize2;
	}
	else
	{
		//	case 3: completely in file 2

		//	Bug fixed by dyx at 2013-4-24: If last file was written in the way of case 1, 
		//	and m_filePos was moved to MAX_FILE_PACKAGE after writting at it happens. Then
		//	next write operation will come to case 3 other than case 2, so we should check whether
		//	m_pFile2 has been existed or not.
		if (!m_pFile2)
		{
			Phase2Open(MAX_FILE_PACKAGE);
			fseek(m_pFile2, 0, SEEK_SET);
		}

		size_t writesize = WriteFile(buffer, size_to_write, m_pFile2);
		m_filePos += writesize;
		if (m_filePos > m_size1 + m_size2)
			m_size2 = m_filePos - m_size1;
		return writesize;
	}

	return 0;
}

void TPackageFile::Seek(int64 offset, int origin)
{
	int64 newpos = m_filePos;

	if (m_pFile2)
	{
		switch (origin)
		{
		case SEEK_SET:
			newpos = offset;
			break;

		case SEEK_CUR:
			newpos = m_filePos + offset;
			break;

		case SEEK_END:
			newpos = m_size1 + m_size2 + offset;
			break;
		}

		if (newpos < 0)
			newpos = 0;
		if (newpos > m_size1 + m_size2)
			newpos = m_size1 + m_size2;

		if (newpos < m_size1)
			fseek(m_pFile1, newpos, SEEK_SET);
		else
			fseek(m_pFile2, newpos - m_size1, SEEK_SET);

		m_filePos = newpos;
	}
	else
	{
		fseek(m_pFile1, offset, origin);
		m_filePos = ftell(m_pFile1);
	}

	return;
}

int64 TPackageFile::Tell()
{
	return m_filePos;
}

void TPackageFile::SetPackageFileSize(int64 fileSize)
{
	if (m_pFile2)
	{
		if (fileSize <= MAX_FILE_PACKAGE)
		{
			int fileHandle = _fileno(m_pFile1);
			_chsize(fileHandle, fileSize);
			m_size1 = fileSize;

			fclose(m_pFile2);
			m_pFile2 = NULL;
			remove(m_path2);
			m_size2 = 0;
		}
		else
		{
			int fileHandle = _fileno(m_pFile2);
			m_size2 = fileSize - MAX_FILE_PACKAGE;
			_chsize(fileHandle, m_size2);
		}
	}
	else
	{
		int fileHandle = _fileno(m_pFile1);
		_chsize(fileHandle, fileSize);
		m_size1 = fileSize;
	}
}

size_t TPackageFile::ReadFile(void* buffer, const size_t num_byte, FILE* stream)
{
	if (!stream || !buffer)
	{
		//	AFERRLOG(("Read Param ERROR! pBuffer:%d, fp:%d, FileSize:%d\n\n", (void*)buffer, (void*)stream, num_byte));
		return 0;
	}

	const long beginOffset = ftell(stream);
	if (beginOffset == -1L)
	{
		TLog::Log(LOG_ERR, "FILE", "ftell ERROR, check whether devices support file seeking!!\n\n");
		return 0;
	}

	unsigned char* pBuf = (unsigned char*)buffer;
	size_t maxOnceReadSize = MAX_RW_SIZE;
	int offset = 0;
	while (offset<num_byte)
	{
		int readSize = std::min(num_byte - offset, maxOnceReadSize);
		int actuallyReadSize = fread(pBuf + offset, 1, readSize, stream);

		if (actuallyReadSize == readSize)
		{
			// 读取成功
			offset += readSize;
		}
		else
		{
			// 调整一次最大读取上限
			if (maxOnceReadSize >= 2)
			{
				maxOnceReadSize >>= 1;
			}

			// 将文件标识归位
			if (fseek(stream, beginOffset + offset, SEEK_SET))
			{
				TLog::Log(LOG_ERR, "FILE", "fseek ERROR, check whether devices support file seeking!!");
				return (size_t)offset;
			}
		}
	}

	return (size_t)offset;
}


size_t TPackageFile::WriteFile(const void* buffer, const size_t num_byte, FILE* stream)
{
	if (!stream || !buffer)
	{
		//	AFERRLOG(("Write Param ERROR! pBuffer:%d, fp:%d, FileSize:%d\n\n", (void*)buffer, (void*)stream, num_byte));
		return 0;
	}

	const long beginOffset = ftell(stream);
	if (beginOffset == -1L)
	{
		TLog::Log(LOG_ERR, "FILE",  "TPackageFile::WriteFile ftell ERROR, check whether devices support file seeking!!");
		return 0;
	}

	const unsigned char* pBuf = (unsigned char*)buffer;
	size_t maxOnceWriteSize = MAX_RW_SIZE;
	int offset = 0;
	while (offset<num_byte)
	{
		int writeSize = std::min(num_byte - offset, maxOnceWriteSize);
		int actuallyWriteSize = fwrite(pBuf + offset, 1, writeSize, stream);

		if (actuallyWriteSize == writeSize)
		{
			// 写入成功
			offset += writeSize;
		}
		else
		{
			// 调整一次最大写入上限
			if (maxOnceWriteSize >= 2)
			{
				maxOnceWriteSize >>= 1;
			}

			// 将文件标识归位
			if (fseek(stream, beginOffset + offset, SEEK_SET))
			{
				TLog::Log(LOG_ERR, "FILE", "TPackageFile::WriteFile: fseek ERROR, check whether devices support file seeking");
				return (size_t)offset;
			}
		}
	}

	return (size_t)offset;
}

