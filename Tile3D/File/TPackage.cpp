#include "TFileDir.h"
#include "TPackage.h"
#include "TPackageFile.h"
#include "TPackageMan.h"
#include "Util/TLog.h"
#include "Util/TAssert.h"
#include "Sys/TSysFile.h"
#include "zlib/zlib.h"
#include "TScriptFile.h"
#include "Core/TMemory.h"

int TPackage::PackageDir::SearchItemIndex(const char * name, int * pos)
{
	int left, right, mid;
	left = 0;
	right = m_list.Size() - 1;
	mid = 0;
	while (left <= right)
	{
		mid = (left + right) / 2;
		int rst = TSysFile::StrCmpNoCase(name, m_list[mid]->_name);
		if (rst < 0)
		{
			right = mid - 1;
		}
		else if (rst > 0)
		{
			left = mid + 1;
		}
		else
		{
			return mid;
		}
	}
	if (pos) *pos = mid;
	return -1;
}

TPackage::PackageEntry* TPackage::PackageDir::SearchItem(const char * name)
{
	int idx = SearchItemIndex(name, nullptr);
	if (idx < 0)
		return NULL;
	else
		return m_list[idx];
}


int TPackage::PackageDir::RemoveItem(const char * name)
{
	int rst;
	rst = SearchItemIndex(name, NULL);
	if (rst < 0) return -1;
	delete m_list[rst];
	m_list.Remove(rst);
	return 0;
}

TPackage::PackageEntry* TPackage::PackageDir::GetItem(int index)
{
	if (index <0 || index >= (int)(m_list.Size())) return NULL;
	return m_list[index];
}


int TPackage::PackageDir::AppendEntry(PackageEntry * item)
{
	int pos;
	if (SearchItemIndex(item->_name, &pos) >= 0)
	{
		//名字重复
		return -1;
	}

	if (pos >= (int)(m_list.Size()))
	{
		m_list.Add(item);
	}
	else
	{
		int rst = TSysFile::StrCmpNoCase(item->_name, m_list[pos]->_name);
		if (rst < 0)
		{
			m_list.Insert(item, pos);
		}
		else
		{
			m_list.Insert(item, pos + 1);
		}
	}
	return 0;
}

int TPackage::PackageDir::SearchEntry(const char * filename)
{
	char name[MAX_PATH];
	strcpy(name, filename);
	char * tok;
	tok = strtok(name, "\\");
	PackageEntry * ent = this;
	while (tok)
	{
		char * next = strtok(NULL, "\\");
		PackageEntry * tmp = ent->SearchItem(tok);
		if (tmp == NULL) return -1;
		if (next)
		{
			if (tmp->IsContainer())
			{
				ent = tmp;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return tmp->GetIndex();
		}
		tok = next;
	}
	return -1;
}


int TPackage::PackageDir::Clear()
{
	for (int i = 0; i< m_list.Size(); i++)
	{
		delete m_list[i];
	}
	m_list.Clear();
	return 0;
}



TPackage::TPackage() 
{
	m_bChanged = false;
	m_bReadOnly = false;
	m_bUseShortName = false;

	m_totalCount = 0;

	memset(&m_header, 0, sizeof(m_header));

	m_pPackageFile = nullptr;
	m_pckFileName[0] = '\0';
	m_folder[0] = '\0';

	m_sharedCount = 0;
	m_cacheCount = 0;

	m_bHasSaferHeader = false;
	memset(&m_safeHeader, 0, sizeof(m_safeHeader));

	m_guardByte0 = 0xfdfdfeee;
	m_guardByte1 = 0xf00dbeef;
	m_maskPasswd = 0xa8937462;
	m_checkMask = 0x59374231;
}

TPackage::~TPackage()
{
}

bool TPackage::Close()
{
	if (m_bChanged)
	{
		int64 fileSize = m_header.m_entryOffset;
		int entrySize = 0;
		if (!SaveEntries(&entrySize))
			return false;
		fileSize += entrySize;

		// Write file header here;
		m_header.m_entryOffset ^= GetMaskPasswd();
		m_pPackageFile->Write(&m_header, sizeof(PackageHeader), 1);
		m_header.m_entryOffset ^= GetMaskPasswd();
		fileSize += sizeof(PackageHeader);

		int numFile = m_fileEntries.Size();
		m_pPackageFile->Write(&numFile, sizeof(int), 1);
		fileSize += sizeof(int);
		m_pPackageFile->Write(&m_header.m_version, sizeof(int), 1);
		fileSize += sizeof(int);

		m_pPackageFile->SetPackageFileSize(fileSize);

		SaveSafeHeader();
		m_bChanged = false;
	}

	if (m_pPackageFile)
	{
		m_pPackageFile->Close();
		delete m_pPackageFile;
		m_pPackageFile = NULL;
	}

	//	Release entries
	for (int i = 0; i < m_fileEntries.Size(); i++)
	{
		if (m_fileEntries[i])
		{
			delete m_fileEntries[i];
			m_fileEntries[i] = NULL;
		}
	}

	//	Release entries cache
	for (int i = 0; i < m_fileEntryCaches.Size(); i++)
	{
		FileEntryCache* pCache = m_fileEntryCaches[i];
		if (pCache)
		{
			if (pCache->m_pEntryCompressed)
			{
				delete pCache->m_pEntryCompressed;
				pCache->m_pEntryCompressed = nullptr;
			}

			delete pCache;
		}

		m_fileEntryCaches[i] = nullptr;
	}

	m_fileEntries.Clear();
	m_fileEntryCaches.Clear();

	//	Release all shared files
	int unClosed = 0;
	THashNode<int, SharedFile*> * pHead = m_sharedFiles.GetHead();
	THashNode<int, SharedFile*> * pNode = pHead;
	THashNode<int, SharedFile*> * pPrevNode;
	while (pNode) {
		SharedFile* pFileItem = pNode->m_value;
		if (pFileItem->m_refCnt > 0) {
			unClosed++;
		}
		delete pFileItem->m_pFileData;
		delete pFileItem;
		pPrevNode = pNode;
		pNode = pNode->m_pNext;
		delete pPrevNode;
	}
	if (unClosed)
		TLog::Log(LOG_INFO, "FILE", "TPackage::Close(), %d file in package weren't closed !", unClosed);

	//	Release cache file list
	THashNode<int, CacheFilename*> * pHead2 = m_cachedFiles.GetHead();
	THashNode<int, CacheFilename*> * pNode2 = pHead2;
	THashNode<int, CacheFilename*> * pPrevNode2;
	while (pNode2) {
		CacheFilename* pFile = pNode2->m_value;
		delete pFile;
		pPrevNode2 = pNode2;
		pNode2 = pNode2->m_pNext;
		delete pPrevNode2;
	}
	return true;
}

bool TPackage::Open(const char* pckPath, bool bEncrypt)
{
	char folder[MAX_PATH];
	strncpy(folder, pckPath, MAX_PATH);
	if (folder[0] == '\0')
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::Open(), can not open a null or empty file name!");
		return false;
	}
	char* pext = folder + strlen(folder) - 1;
	while (pext != folder)
	{
		if (*pext == '.')
			break;

		pext--;
	}

	if (pext == folder)
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::Open(), only file with extension can be opened!");
		return false;
	}

	*pext++ = '\\';
	*pext = '\0';

	return Open(pckPath, folder, bEncrypt, false);
}




bool TPackage::Open(const char* pckPath, const char* folder, bool bEncrypt, bool bShortName)
{
	TLog::Log(LOG_INFO, "FILE", "TPackage::Open(), start open pck file pck_path=%s, folder=%s", pckPath, folder);
	char fullPckPath[MAX_PATH];
	TFileDir::GetInstance()->GetFullPath(fullPckPath, pckPath);

	m_bUseShortName = bShortName;

	//	Save folder name
	TAssert(folder);
	int folderLen = strlen(folder);
	memset(m_folder, 0, sizeof(m_folder));

	if (folderLen > 0)
	{
		strncpy(m_folder, folder, MAX_PATH);
		TSysFile::StrToLower(m_folder);
		TFileDir::GetInstance()->NormalizeFileName(m_folder);

		//	Add '\' at folder tail
		if (m_folder[folderLen - 1] != '\\')
		{
			m_folder[folderLen] = '\\';
			m_folder[folderLen + 1] = '\0';
		}
	}

	m_bReadOnly = false;
	m_pPackageFile = new TPackageFile();
	if (!m_pPackageFile->Open(fullPckPath, "r+b"))
	{
		if (!m_pPackageFile->Open(fullPckPath, "rb"))
		{
			delete m_pPackageFile;
			m_pPackageFile = nullptr;

			TLog::Log(LOG_ERR, "FILE", "TPackage::Open, Can not open file [%s]", fullPckPath);
			return false;
		}
		m_bReadOnly = true;
	}

	strncpy(m_pckFileName, pckPath, MAX_PATH);

	if (!LoadSafeHeader()) {
		delete m_pPackageFile;
		return false;
	}

	int64 offset;
	m_pPackageFile->Seek(0, SEEK_END);
	offset = m_pPackageFile->Tell();
	m_pPackageFile->Seek(0, SEEK_SET);

	if (m_bHasSaferHeader)
		offset = m_safeHeader.m_offset;

	// Now analyse the file entries of the package;
	int version;
	// First version;
	m_pPackageFile->Seek(offset - sizeof(int), SEEK_SET);
	m_pPackageFile->Read(&version, sizeof(int), 1);

	if (version == PCK_VERSION)
	{
		int numFile;

		// Now read file number;
		m_pPackageFile->Seek(offset - (sizeof(int) + sizeof(int)), SEEK_SET);
		m_pPackageFile->Read(&numFile, sizeof(int), 1);
		m_pPackageFile->Seek(offset - (sizeof(PackageHeader) + sizeof(int) + sizeof(int)), SEEK_SET);
		m_pPackageFile->Read(&m_header, sizeof(PackageHeader), 1);
		if (strstr(m_header.m_description, "lica File Package") == NULL)
			return false;
		strncpy(m_header.m_description, PCK_COPYRIGHT_TAG, sizeof(m_header.m_description));

		// if we don't expect one encrypt package, we will let the error come out.
		// make sure the encrypt flag is correct
		bool bPackIsEncrypt = (m_header.m_flags & PCK_FLAG_ENCRYPT) != 0;
		if (bEncrypt != bPackIsEncrypt)
		{
			TLog::Log(LOG_ERR, "FILE", "TPackage::Open, wrong encrypt flag");
			return false;
		}

		m_header.m_entryOffset ^= GetMaskPasswd();

		if (m_header.m_guardByte0 != GetGuardByte0() ||
			m_header.m_guardByte1 != GetGuardByte1())
		{
			// corrput file
			TLog::Log(LOG_ERR, "FILE", "TPackage::Open, GuardBytes corrupted [%s]", pckPath);
			return false;
		}

		//	Seek to entry list;
		m_pPackageFile->Seek(m_header.m_entryOffset, SEEK_SET);

		//	Create entries
		m_fileEntries.Reserve(numFile);
		m_fileEntryCaches.Reserve(numFile);
		m_totalCount = numFile;
		for (int i = 0; i < numFile; i++)
		{
			FileEntry* pEntry = new FileEntry;
			pEntry->m_accessCnt = 0;

			FileEntryCache* pEntryCache = new FileEntryCache;
			memset(pEntryCache, 0, sizeof(FileEntryCache));

			// first read the entry size after compressed
			int nCompressedSize;
			m_pPackageFile->Read(&nCompressedSize, sizeof(int), 1);
			nCompressedSize ^= GetMaskPasswd();

			int checkSize;
			m_pPackageFile->Read(&checkSize, sizeof(int), 1);
			checkSize = checkSize ^ GetCheckMask() ^ GetMaskPasswd();

			if (nCompressedSize != checkSize)
			{
				TLog::Log(LOG_ERR, "FILE", "TPackage::InnerOpen, Check Byte Error!");
				return false;
			}

			pEntryCache->m_compressedLength = nCompressedSize;
			pEntryCache->m_pEntryCompressed = new char[nCompressedSize];

			m_pPackageFile->Read(pEntryCache->m_pEntryCompressed, nCompressedSize, 1);
			unsigned long entrySize = sizeof(FileEntry);
			if (entrySize == nCompressedSize)
			{
				memcpy(pEntry, pEntryCache->m_pEntryCompressed, sizeof(FileEntry));

				// maybe the original package FileEntry has not been compressed
				unsigned long compressedSize = sizeof(FileEntry);
				unsigned char * pBuffer = new unsigned char[sizeof(FileEntry)];
				int nRet = Compress((unsigned char*)pEntry, sizeof(FileEntry), pBuffer, &compressedSize);
				if (nRet != 0 || compressedSize >= sizeof(FileEntry))
				{
					compressedSize = sizeof(FileEntry);
					memcpy(pBuffer, pEntry, sizeof(FileEntry));
				}
				pEntryCache->m_compressedLength = compressedSize;
				pEntryCache->m_pEntryCompressed = new (pEntryCache->m_pEntryCompressed) char[compressedSize];
				memcpy(pEntryCache->m_pEntryCompressed, pBuffer, compressedSize);
				delete[] pBuffer;
			}
			else
			{
				if (0 != Uncompress((unsigned char*)(pEntryCache->m_pEntryCompressed), nCompressedSize, (unsigned char*)pEntry, &entrySize))
				{
					TLog::Log(LOG_ERR, "FILE", "TPackage::InnerOpen,decode file entry fail!");
					return false;
				}

				TAssert(entrySize == sizeof(FileEntry));
			}

			//	Note: A bug existed in AppendFileCompressed() after m_bUseShortName was introduced. The bug
			//		didn't normalize file name when new file is added to package, so that the szFileName of
			//		FileEntry may contain '/' character. The bug wasn't fixed until 2013.3.18, many 'new' files
			//		have been added to package, so NormalizeFileName is inserted here to ensure all szFileName
			//		of FileEntry uses '\' instead of '/', at least in memory.
			TFileDir::GetInstance()->NormalizeFileName(pEntry->m_fileName);

			m_fileEntries.Add(pEntry);
			m_fileEntryCaches.Add(pEntryCache);
		}

		ResortEntries();

		// now we move entry point to the end of the file so to keep old entries here
		if (m_bHasSaferHeader)
			m_header.m_entryOffset = offset;
	}
	else
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::InnerOpen,Incorrect version!");
		return false;
	}


	m_bChanged = false;
	m_sharedCount = 0;
	m_cacheCount = 0;

	TLog::Log(LOG_INFO, "FILE", "TPackage::Open(), finish open pck file pck_path=%s, folder=%s", pckPath, folder);
	return true;
}

bool TPackage::Create(const char* pckPath, const char *folder, bool bEncrypt)
{
	char fullPckPath[MAX_PATH];
	TFileDir::GetInstance()->GetFullPath(fullPckPath, pckPath);

	m_bUseShortName = true;

	//	Save folder name
	TAssert(folder);
	int folderLen = strlen(folder);
	memset(m_folder, 0, sizeof(m_folder));

	if (folderLen > 0)
	{
		strncpy(m_folder, folder, MAX_PATH);
		TSysFile::StrToLower(m_folder);
		TFileDir::GetInstance()->NormalizeFileName(m_folder);

		//	Add '\' at folder tail
		if (m_folder[folderLen - 1] != '\\')
		{
			m_folder[folderLen] = '\\';
			m_folder[folderLen + 1] = '\0';
		}
	}

	m_bReadOnly = false;
	m_pPackageFile = new TPackageFile();
	if (!m_pPackageFile->Open(fullPckPath, "wb"))
	{
		delete m_pPackageFile;
		m_pPackageFile = NULL;
		TLog::Log(LOG_ERR, "FILE", "TPackage::Create(), Can not create file [%s]", fullPckPath);
		return false;
	}
	strncpy(m_pckFileName, pckPath, MAX_PATH);

	CreateSafeHeader();

	// Init header;
	memset(&m_header, 0, sizeof(PackageHeader));
	m_header.m_guardByte0 = m_guardByte0;
	m_header.m_entryOffset = sizeof(SafePackageHeader);
	m_header.m_version = PCK_VERSION;
	m_header.m_flags = bEncrypt ? PCK_FLAG_ENCRYPT : 0;
	m_header.m_guardByte1 = m_guardByte1;
	strncpy(m_header.m_description, PCK_COPYRIGHT_TAG, sizeof(m_header.m_description));

	m_fileEntries.Clear();
	m_fileEntryCaches.Clear();

	m_bChanged = false;
	m_sharedCount = 0;
	m_cacheCount = 0;
	return true;
}

bool TPackage::GetFileEntry(const char* fileName, FileEntry* pFileEntry, int* pIndex)
{
	char findName[MAX_PATH];

	//	Normalize file name
	strncpy(findName, fileName, MAX_PATH);
	TFileDir::GetInstance()->NormalizeFileName(findName);
	if (m_bUseShortName) {
		char fullName[MAX_PATH];
		strcpy(fullName, findName);
		TFileDir::GetInstance()->GetRelativePathNoBase(fullName, m_folder, findName);
	}

	memset(pFileEntry, 0, sizeof(FileEntry));
	int iEntry = m_directory.SearchEntry(findName);
	if (iEntry < 0)
		return false;

	if (!m_fileEntries[iEntry])
		return false;

	*pFileEntry = *m_fileEntries[iEntry];

	if (!CheckFileEntryValid(pFileEntry))
	{
		pFileEntry->m_length = 0;
		pFileEntry->m_compressedLength = 0;
	}

	if (pIndex)
		*pIndex = iEntry;

	return true;
}

bool TPackage::CheckFileEntryValid(FileEntry* pFileEntry)
{
	if (pFileEntry->m_compressedLength > MAX_FILE_PACKAGE)
	{
		// patcher在更新pck的时候，如果遇到了文件长度为0的文件，会给进一个错误的pFileEntry->dwCompressedLength
		// 通常情况下该值为0xFFFFFFFC，但是也可能有其他情况被写成了其他的值，正常情况下一个包里的小文件不应该太大
		// 我们认为过大的FileEntry是错的 
		// 标准：大于MAX_FILE_PACKAGE的情况都认为是过大的，因为pck文件大于该值时会自动拆包，理论上不会有任何一个文件大于这个值
		TLog::Log(LOG_ERR, "FILE", "TPackage::CheckFileEntryValid, file entry [%s]'s length is not correct!", pFileEntry->m_fileName);
		return false;
	}

	return true;
}

bool TPackage::IsFileExist(const char* fileName)
{
	FileEntry fileEntry;
	int index;
	return GetFileEntry(fileName, &fileEntry, &index);
}

TPackage::PackageDir * TPackage::GetDirEntry(const char * path)
{
	char findName[MAX_PATH];
	int len, i;
	char *name, *tok;

	strncpy(findName, path, MAX_PATH);
	TSysFile::StrToLower(findName);
	name = findName;
	len = strlen(findName);
	for (i = 0; i<len; i++)
	{
		if (findName[i] == '/')
			findName[i] = '\\';
	}
	tok = strtok(name, "\\");
	PackageDir * pDir = &m_directory;
	while (tok && *tok)
	{
		PackageEntry * pEnt = pDir->SearchItem(tok);
		if (pEnt == NULL) return NULL; //entry not found
		if (!pEnt->IsContainer()) return NULL;
		tok = strtok(NULL, "\\");
		pDir = (PackageDir*)pEnt;
	}
	return pDir;
}

bool TPackage::InsertFileToDir(const char * filename, int index)
{
	char findName[MAX_PATH];
	int len;
	char *name, *tok;

	strncpy(findName, filename, MAX_PATH);
	TSysFile::StrToLower(findName);
	name = findName;
	len = strlen(findName);
	for (int i = 0; i<len; i++)
	{
		if (findName[i] == '/')
			findName[i] = '\\';
	}
	tok = strtok(name, "\\");
	PackageDir * pDir = &m_directory;
	while (tok)
	{
		char * next = strtok(NULL, "\\");
		PackageEntry * pEnt = pDir->SearchItem(tok);
		if (next)
		{
			if (pEnt == NULL)
			{
				PackageDir *tmp = new PackageDir(tok);
				pDir->AppendEntry(tmp);
				pDir = tmp;
			}
			else
			{
				TAssert(pEnt->IsContainer());
				if (!pEnt->IsContainer())
				{
					TLog::Log(LOG_ERR, "FILE", "TPackage::InsertFileToDir(), Directory conflict:%s", filename);
					return false;
				}
				pDir = (PackageDir*)pEnt;
			}
		}
		else
		{
			if (pEnt == NULL)
			{
				pDir->AppendEntry(new PackageFile(tok, index));
			}
			else
			{
				TAssert(!pEnt->IsContainer());
				if (pEnt->IsContainer())
					return false;
				else
					((PackageFile*)pEnt)->SetIndex(index);
				break;
			}
		}
		tok = next;
	}
	return true;
}



bool TPackage::RemoveFileFromDir(const char * filename)
{
	char szFindName[MAX_PATH];
	int nLength, i;
	char *name, *tok;

	strncpy(szFindName, filename, MAX_PATH);
	TSysFile::StrToLower(szFindName);
	name = szFindName;
	nLength = strlen(szFindName);
	for (i = 0; i<nLength; i++)
	{
		if (szFindName[i] == '/')
			szFindName[i] = '\\';
	}
	tok = strtok(name, "\\");
	PackageDir * pDir = &m_directory;
	while (tok)
	{
		PackageEntry * pEnt = pDir->SearchItem(tok);
		if (pEnt == NULL) return false; //entry not found
		char * next = strtok(NULL, "\\");
		if (next == NULL)
		{
			if (!pEnt->IsContainer())
			{
				pDir->RemoveItem(tok);
				return true;
			}
			return false;
		}
		else
		{
			if (pEnt->IsContainer())
				pDir = (PackageDir *)pEnt;
			else
				return false;
		}
		tok = next;
	}
	return false;
}


bool TPackage::AppendFile(const char* fileName, unsigned char* pFileBuffer, unsigned long fileLength, bool bCompress)
{
	// We should use a function to check whether szFileName has been added into the package;
	if (m_bReadOnly)
	{
		TLog::Log(LOG_ERR, "FILE", "AFilePackage::AppendFile(), Read only package, can not append!");
		return false;
	}

	FileEntry fileEntry;
	int index;
	if (GetFileEntry(fileName, &fileEntry, &index))
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::AppendFile(), file entry [%s] already exist!", fileName);
		return false;
	}

	unsigned long compressedLength = fileLength;
	if (bCompress)
	{
		//	Compress the file
		unsigned char* pBuffer = new unsigned char[fileLength];
		if (!pBuffer)
			return false;

		if (0 != Compress(pFileBuffer, fileLength, pBuffer, &compressedLength))
		{
			//compress error, so use uncompressed format
			compressedLength = fileLength;
		}

		if (compressedLength < fileLength)
		{
			if (!AppendFileCompressed(fileName, pBuffer, fileLength, compressedLength))
			{
				delete[] pBuffer;
				return false;
			}
		}
		else
		{
			if (!AppendFileCompressed(fileName, pFileBuffer, fileLength, fileLength))
			{
				delete[] pBuffer;
				return false;
			}
		}

		delete[] pBuffer;
	}
	else
	{
		if (!AppendFileCompressed(fileName, pFileBuffer, fileLength, fileLength))
			return false;
	}

	return true;
}

bool TPackage::AppendFileCompressed(const char* fileName, unsigned char* pCompressedFileBuffer, unsigned long fileLength, unsigned long compressedLength)
{
	FileEntry* pEntry = new FileEntry();

	//	Normalize file name
	char savedFileName[MAX_PATH];
	strcpy(savedFileName, fileName);
	TFileDir::GetInstance()->NormalizeFileName(savedFileName);
	if (m_bUseShortName) {
		char fullName[MAX_PATH];
		strcpy(fullName, savedFileName);
		TFileDir::GetInstance()->GetRelativePathNoBase(fullName, m_folder, savedFileName);
	}

	fileName = savedFileName;

	//	Store this file;
	strncpy(pEntry->m_fileName, fileName, MAX_PATH);
	pEntry->m_offset = m_header.m_entryOffset;
	pEntry->m_length = fileLength;
	pEntry->m_compressedLength = compressedLength;
	pEntry->m_accessCnt = 0;
	if (!CheckFileEntryValid(pEntry))
	{
		delete pEntry;
		TLog::Log(LOG_ERR, "FILE", "TPackage::AppendFile(), Invalid File Entry!");
		return false;
	}

	m_fileEntries.Add(pEntry);

	FileEntryCache* pEntryCache = new FileEntryCache();
	unsigned long compressedSize = sizeof(FileEntry);
	unsigned char * pBuffer = new unsigned char[sizeof(FileEntry)]; 
	int nRet = Compress((unsigned char*)pEntry, sizeof(FileEntry), pBuffer, &compressedSize);
	if (nRet != 0 || compressedSize >= sizeof(FileEntry))
	{
		compressedSize = sizeof(FileEntry);
		memcpy(pBuffer, pEntry, sizeof(FileEntry));
	}
	pEntryCache->m_compressedLength = compressedSize;
	pEntryCache->m_pEntryCompressed = (char *)new char[compressedSize];
	memcpy(pEntryCache->m_pEntryCompressed, pBuffer, compressedSize);
	m_fileEntryCaches.Add(pEntryCache);
	delete[] pBuffer;

	m_pPackageFile->Seek(m_header.m_entryOffset, SEEK_SET);

	//	We write the compressed buffer into the disk;
	Encrypt(pCompressedFileBuffer, compressedLength);
	m_pPackageFile->Write(pCompressedFileBuffer, compressedLength, 1);
	Decrypt(pCompressedFileBuffer, compressedLength);
	m_header.m_entryOffset += compressedLength;

	InsertFileToDir(fileName, m_fileEntries.Size() - 1);
	m_bChanged = true;
	return true;
}


bool TPackage::RemoveFile(const char* fileName)
{
	if (m_bReadOnly)
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::RemoveFile(), Read only package, can not remove file!");
		return false;
	}

	FileEntry Entry;
	int	nIndex;
	if (!GetFileEntry(fileName, &Entry, &nIndex))
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::RemoveFile(), Can not find file %s", fileName);
		return false;
	}

	FileEntry* pEntry = m_fileEntries[nIndex];
	RemoveFileFromDir(pEntry->m_fileName);

	//	Added by dyx on 2013.10.14. Now we only delete entry object and leave a NULL at it's position
	//	in m_aFileEntries, this is in order that the entry indices recoreded in file items of m_directory 
	//	can still be valid and needn't updating.
	delete pEntry;
	m_fileEntries[nIndex] = NULL;

	FileEntryCache* pEntryCache = m_fileEntryCaches[nIndex];
	if (pEntryCache)
	{
		if (pEntryCache->m_pEntryCompressed) {
			delete[] pEntryCache->m_pEntryCompressed;
		}
		delete pEntryCache;
		m_fileEntryCaches[nIndex] = NULL;
	}

	//	ResortEntries();

	m_bChanged = true;
	return true;
}

bool TPackage::ReplaceFile(const char* fileName, unsigned char* pFileBuffer, unsigned long fileLength, bool bCompress)
{
	//	We only add a new file copy at the end of the file part, and modify the 
	//	file entry point to that file body;
	unsigned long compressedLength = fileLength;

	if (bCompress)
	{
		//	Try to compress the file
		unsigned char* pBuffer = new unsigned char[fileLength];
		if (!pBuffer)
			return false;

		if (0 != Compress(pFileBuffer, fileLength, pBuffer, &compressedLength))
		{
			//compress error, so use uncompressed format
			compressedLength = fileLength;
		}

		if (compressedLength < fileLength)
		{
			if (!ReplaceFileCompressed(fileName, pBuffer, fileLength, compressedLength))
			{
				delete[] pBuffer;
				return false;
			}
		}
		else
		{
			if (!ReplaceFileCompressed(fileName, pFileBuffer, fileLength, fileLength))
			{
				delete[] pBuffer;
				return false;
			}
		}

		delete[] pBuffer;
	}
	else
	{
		if (!ReplaceFileCompressed(fileName, pFileBuffer, fileLength, fileLength))
			return false;
	}


	return true;
}

bool TPackage::ReplaceFileCompressed(const char * fileName, unsigned char* pCompressedBuffer, unsigned long fileLength, unsigned long compressedLength)
{
	if (m_bReadOnly)
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::ReplaceFileCompressed, Read only package, can not replace!");
		return false;
	}

	FileEntry entry;
	int	nIndex;
	if (!GetFileEntry(fileName, &entry, &nIndex))
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::ReplaceFile(), Can not find file %s", fileName);
		return false;
	}

	entry.m_offset = m_header.m_entryOffset;
	entry.m_length = fileLength;
	entry.m_compressedLength = compressedLength;
	if (!CheckFileEntryValid(&entry))
	{
		// 先行检查输入参数是否合法，如果不合法，则提前返回，不做任何修改
		TLog::Log(LOG_ERR, "FILE", "TPackage::ReplaceFile(), Invalid File Entry");
		return false;
	}

	FileEntry* pEntry = m_fileEntries[nIndex];
	TAssert(pEntry);

	// modify this file entry to point to the new file body;			
	pEntry->m_offset = m_header.m_entryOffset;
	pEntry->m_length = fileLength;
	pEntry->m_compressedLength = compressedLength;

	FileEntryCache* pEntryCache = m_fileEntryCaches[nIndex];
	unsigned long compressedSize = sizeof(FileEntry);
	unsigned char * pBuffer = new unsigned char[sizeof(FileEntry)];
	int nRet = Compress((unsigned char*)pEntry, sizeof(FileEntry), pBuffer, &compressedSize);
	if (nRet != 0 || compressedSize >= sizeof(FileEntry))
	{
		compressedSize = sizeof(FileEntry);
		memcpy(pBuffer, pEntry, sizeof(FileEntry));
	}
	pEntryCache->m_compressedLength = compressedSize;
	pEntryCache->m_pEntryCompressed = new(pEntryCache->m_pEntryCompressed)char[compressedSize];
	memcpy(pEntryCache->m_pEntryCompressed, pBuffer, compressedSize);
	delete[] pBuffer;

	m_pPackageFile->Seek(m_header.m_entryOffset, SEEK_SET);

	//	We write the compressed buffer into the disk;
	Encrypt(pCompressedBuffer, compressedLength);
	m_pPackageFile->Write(pCompressedBuffer, compressedLength, 1);
	Decrypt(pCompressedBuffer, compressedLength);
	m_header.m_entryOffset += compressedLength;

	m_bChanged = true;
	return true;
}



bool TPackage::ReadFile(const char* szFileName, unsigned char* pFileBuffer, unsigned long* pbufferLen)
{
	FileEntry fileEntry;
	int index;
	if (!GetFileEntry(szFileName, &fileEntry, &index))
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::ReadFile(), Can not find file entry [%s]!", szFileName);
		return false;
	}

	return ReadFile(fileEntry, pFileBuffer, pbufferLen);
}

bool TPackage::ReadFile(FileEntry & fileEntry, unsigned char* pFileBuffer, unsigned long * pBufferLen)
{
	if (*pBufferLen < fileEntry.m_length)
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::ReadFile(), Buffer is too small!");
		return false;
	}

	// We can automaticly determine whether compression has been used;
	if (fileEntry.m_length > fileEntry.m_compressedLength)
	{
		unsigned long fileLength = fileEntry.m_length;
		unsigned char* pBuffer = new unsigned char[fileEntry.m_compressedLength];
		if (!pBuffer)
			return false;

		m_readFileLock.Lock();
		m_pPackageFile->Seek(fileEntry.m_offset, SEEK_SET);
		m_pPackageFile->Read(pBuffer, fileEntry.m_compressedLength, 1);
		Decrypt(pBuffer, fileEntry.m_compressedLength);
		m_readFileLock.Unlock();


		if (0 != Uncompress(pBuffer, fileEntry.m_compressedLength, pFileBuffer, &fileLength))
		{
			FILE * fp = fopen("logs\\bad.dat", "wb");
			if (fp)
			{
				fwrite(pBuffer, fileEntry.m_compressedLength, 1, fp);
				fclose(fp);
			}

			delete[] pBuffer;
			return false;
		}

		//uncompress(pFileBuffer, &dwFileLength, m_pBuffer, fileEntry.dwCompressedLength);

		*pBufferLen = fileLength;
		delete[] pBuffer;
	}
	else
	{
		m_readFileLock.Lock();
		m_pPackageFile->Seek(fileEntry.m_offset, SEEK_SET);
		m_pPackageFile->Read(pFileBuffer, fileEntry.m_length, 1);
		Decrypt(pFileBuffer, fileEntry.m_length);
		m_readFileLock.Unlock();

		*pBufferLen = fileEntry.m_length;
	}

	return true;
}


bool TPackage::ReadCompressedFile(const char* fileName, unsigned char* pCompressedBuffer, unsigned long* pBufferLen)
{
	FileEntry fileEntry;

	int index;
	if (!GetFileEntry(fileName, &fileEntry, &index))
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::ReadCompressedFile(), Can not find file entry [%s]!", fileName);
		return false;
	}

	return ReadCompressedFile(fileEntry, pCompressedBuffer, pBufferLen);
}

bool TPackage::ReadCompressedFile(FileEntry& fileEntry, unsigned char* pCompressedBuffer, unsigned long * pdwBufferLen)
{
	if (*pdwBufferLen < fileEntry.m_compressedLength)
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::ReadCompressedFile(), Buffer is too small!");
		return false;
	}

	m_readFileLock.Lock();
	m_pPackageFile->Seek(fileEntry.m_offset, SEEK_SET);
	*pdwBufferLen = m_pPackageFile->Read(pCompressedBuffer, 1, fileEntry.m_compressedLength);
	Decrypt(pCompressedBuffer, fileEntry.m_compressedLength);
	m_readFileLock.Unlock();
	return true;
}

void* Zlib_User_Alloc(void* opaque, unsigned int items, unsigned int size)
{
	return new char[size*items];
}

void Zlib_User_Free(void* opaque, void* ptr)
{
	delete[] ptr;
}


int Zlib_Compress(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen, int level = Z_BEST_SPEED)
{
	z_stream stream;
	int err;

	stream.next_in = (Bytef*)source;
	stream.avail_in = (uInt)sourceLen;
#ifdef MAXSEG_64K
	/* Check for source > 64K on 16-bit machine: */
	if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;
#endif
	stream.next_out = dest;
	stream.avail_out = (uInt)*destLen;
	if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

	stream.zalloc = &Zlib_User_Alloc;	//0;
	stream.zfree = &Zlib_User_Free;		//0;
	stream.opaque = (voidpf)0;

	err = deflateInit(&stream, level);
	if (err != Z_OK) return err;

	err = deflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END) {
		deflateEnd(&stream);
		return err == Z_OK ? Z_BUF_ERROR : err;
	}
	*destLen = stream.total_out;

	err = deflateEnd(&stream);
	return err;
}

int Zlib_UnCompress(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
	z_stream stream;
	int err;

	stream.next_in = (Bytef*)source;
	stream.avail_in = (uInt)sourceLen;
	/* Check for source > 64K on 16-bit machine: */
	if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;

	stream.next_out = dest;
	stream.avail_out = (uInt)*destLen;
	if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

	stream.zalloc = &Zlib_User_Alloc;  //0;
	stream.zfree = &Zlib_User_Free;   //0;

	err = inflateInit(&stream);
	if (err != Z_OK) return err;

	err = inflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END) {
		inflateEnd(&stream);
		return err == Z_OK ? Z_BUF_ERROR : err;
	}
	*destLen = stream.total_out;

	err = inflateEnd(&stream);
	return err;
}

/*
Compress a data buffer
pFileBuffer		IN		buffer contains data to be compressed
fileLength		IN		the bytes in buffer to be compressed
pCompressedBuffer	OUT		the buffer to hold the compressed data
pCompressedLength IN/OUT	the compressed buffer size when used as input when out, it contains the real compressed length

RETURN: 0,		ok
-1,		dest buffer is too small
-2,		unknown error
*/
int TPackage::Compress(unsigned char* pFileBuffer, int fileLength, unsigned char* pCompressedBuffer, unsigned long * pCompressedLength)
{
	int nRet = compress2(pCompressedBuffer, pCompressedLength, pFileBuffer, fileLength, 1);
	if (Z_OK == nRet)
		return 0;

	if (Z_BUF_ERROR == nRet)
		return -1;
	else
		return -2;
}


int TPackage::Uncompress(unsigned char* pCompressedBuffer, int compressedLength, unsigned char* pFileBuffer, unsigned long * pFileLength)
{
	int nRet = Zlib_UnCompress(pFileBuffer, pFileLength, pCompressedBuffer, compressedLength);
	if (Z_OK == nRet)
		return 0;

	if (Z_BUF_ERROR == nRet)
		return -1;
	else
		return -2;
}

void TPackage::Encrypt(unsigned char* pBuffer, unsigned long length)
{
	if ((m_header.m_flags & PCK_FLAG_ENCRYPT) == 0)
		return;

	unsigned long mask = length + 0x739802ab;
	for (unsigned long i = 0; i<length; i += 4)
	{
		if (i + 3 < length)
		{
			unsigned long data = (pBuffer[i] << 24) | (pBuffer[i + 1] << 16) | (pBuffer[i + 2] << 8) | pBuffer[i + 3];
			data ^= mask;
			data = (data << 16) | ((data >> 16) & 0xffff);
			pBuffer[i] = (data >> 24) & 0xff;
			pBuffer[i + 1] = (data >> 16) & 0xff;
			pBuffer[i + 2] = (data >> 8) & 0xff;
			pBuffer[i + 3] = data & 0xff;
		}
	}
}

void TPackage::Decrypt(unsigned char* pBuffer, unsigned long length)
{
	if ((m_header.m_flags & PCK_FLAG_ENCRYPT) == 0)
		return;

	unsigned long mask = length + 0x739802ab;

	for (unsigned long i = 0; i< length; i += 4)
	{
		if (i + 3 < length)
		{
			unsigned long data = (pBuffer[i] << 24) | (pBuffer[i + 1] << 16) | (pBuffer[i + 2] << 8) | pBuffer[i + 3];
			data = (data << 16) | ((data >> 16) & 0xffff);
			data ^= mask;
			pBuffer[i] = (data >> 24) & 0xff;
			pBuffer[i + 1] = (data >> 16) & 0xff;
			pBuffer[i + 2] = (data >> 8) & 0xff;
			pBuffer[i + 3] = data & 0xff;
		}
	}
}

/*
Safe Header section
*/
bool TPackage::LoadSafeHeader()
{
	m_pPackageFile->Seek(0, SEEK_SET);

	SafePackageHeaderOld oldFileHeader;
	m_pPackageFile->Read(&oldFileHeader, sizeof(SafePackageHeaderOld), 1);

	if (oldFileHeader.m_tag1 == 0x4dca23ef && oldFileHeader.m_tag2 == 0x56a089b7) {
		m_safeHeader.m_tag = oldFileHeader.m_tag1;
		m_safeHeader.m_offset = oldFileHeader.m_offset;
		m_bHasSaferHeader = true;
	}
	//Currently only support to 8G
	else if(oldFileHeader.m_tag1 == 0x4dca23ef && oldFileHeader.m_tag2 <= 1){
		m_safeHeader.m_tag = oldFileHeader.m_tag1;
		m_safeHeader.m_offset = ((int64)oldFileHeader.m_tag2 << 32) + oldFileHeader.m_offset;

		if (m_safeHeader.m_offset < 0) return false;
		m_bHasSaferHeader = true;
	}
	else {
		m_bHasSaferHeader = false;
	}

	if (m_bHasSaferHeader)
		m_pPackageFile->Phase2Open(m_safeHeader.m_offset);

	m_pPackageFile->Seek(0, SEEK_SET);
	return true;
}

bool TPackage::CreateSafeHeader()
{
	m_bHasSaferHeader = true;

	m_safeHeader.m_tag = 0x4DCA23EF;
	m_safeHeader.m_offset = 0;

	return true;
}

bool TPackage::SaveSafeHeader()
{
	if (m_bHasSaferHeader)
	{
		m_pPackageFile->Seek(0, SEEK_END);
		m_safeHeader.m_offset = m_pPackageFile->Tell();

		m_pPackageFile->Seek(0, SEEK_SET);
		m_pPackageFile->Write(&m_safeHeader, sizeof(SafePackageHeader), 1);
		m_pPackageFile->Seek(0, SEEK_SET);
	}

	return true;
}

bool TPackage::ResortEntries()
{
	m_directory.Clear();
	for (int i = 0; i < m_fileEntries.Size(); i++)
	{
		if (m_fileEntries[i])
		{
			InsertFileToDir(m_fileEntries[i]->m_fileName, i);
		}
	}
	return true;
}

#define ENTRY_BUFFER_SIZE		(1024 * 1024)

bool TPackage::SaveEntries(int * pEntrySize)
{
	int totalSize = 0;
	int numFile = m_fileEntries.Size();

	//Remove NULL entries at first, see RemoveFile for detail.
	for (int i = numFile - 1; i >= 0; i--)
	{
		FileEntry* pEntry = m_fileEntries[i];
		if (!pEntry)
		{
			m_fileEntries.Remove(i);
			TAssert(!m_fileEntryCaches[i]);
			m_fileEntryCaches.Remove(i);
		}
	}

	numFile = m_fileEntries.Size();

	int bufferUsed = 0;
	char * pEntryBuffer = new char[ENTRY_BUFFER_SIZE];
	if (NULL == pEntryBuffer)
		return false;

	// Rewrite file entries and file header here;
	m_pPackageFile->Seek(m_header.m_entryOffset, SEEK_SET);
	for (int i = 0; i < numFile; i++)
	{
		FileEntry* pEntry = m_fileEntries[i];
		FileEntryCache* pEntryCache = m_fileEntryCaches[i];

		if (bufferUsed + sizeof(FileEntry) + sizeof(int) + sizeof(int) > ENTRY_BUFFER_SIZE)
		{
			// flush entry buffer;
			m_pPackageFile->Write(pEntryBuffer, bufferUsed, 1);
			bufferUsed = 0;
		}

		int compressedSize = pEntryCache->m_compressedLength;

		compressedSize ^= GetMaskPasswd();
		memcpy(&pEntryBuffer[bufferUsed], &compressedSize, sizeof(int));
		bufferUsed += sizeof(int);

		compressedSize ^= GetCheckMask();
		memcpy(&pEntryBuffer[bufferUsed], &compressedSize, sizeof(int));
		bufferUsed += sizeof(int);

		memcpy(&pEntryBuffer[bufferUsed], pEntryCache->m_pEntryCompressed, pEntryCache->m_compressedLength);
		bufferUsed += pEntryCache->m_compressedLength;

		totalSize += sizeof(int) + sizeof(int) + pEntryCache->m_compressedLength;
	}

	if (bufferUsed)
	{
		// flush entry buffer;
		m_pPackageFile->Write(pEntryBuffer, bufferUsed, 1);
		bufferUsed = 0;
	}

	delete[] pEntryBuffer;
	pEntryBuffer = NULL;

	if (pEntrySize)
		*pEntrySize = totalSize;
	return true;
}

//	Add a file name to file cache list
bool TPackage::AddCacheFileName(const char* fileName)
{
	CacheFilename* pCacheFile = new CacheFilename();
	if (!pCacheFile)
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::AddCacheFileName, Not enough memory !");
		return false;
	}

	pCacheFile->m_fileName = fileName;
	pCacheFile->m_fileID = TFileDir::GetInstance()->GetIDFromFileName(fileName);

	if(!m_cachedFiles.Put(pCacheFile->m_fileID, pCacheFile))
	{
		//	Failed to put item into table, this maybe caused by file name collision
		delete pCacheFile;
		return false;
	}

	return true;
}


//	Add a group of file names to file cache list
bool TPackage::AddCacheFileNameList(const char* descFile)
{
	TScriptFile scriptFile;
	if (!scriptFile.Open(descFile))
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::AddCacheFileNameList, Failed to open file %s !", descFile);
		return false;
	}

	while (scriptFile.GetNextToken(true))
	{
		AddCacheFileName(scriptFile.m_token);
	}

	scriptFile.Close();
	return true;
}


//	Search a cache file name from list
//	Return index of file name for success, otherwise return -1
TPackage::CacheFilename* TPackage::SearchCacheFileName(const char* szFileName)
{
	int fileID = TFileDir::GetInstance()->GetIDFromFileName(szFileName);
	return SearchCacheFileName(fileID);
}


//	Search a cache file name from list
//	Return index of file name for success, otherwise return -1
TPackage::CacheFilename* TPackage::SearchCacheFileName(int fileID)
{
	CacheFilename * pCacheFileName = *(m_cachedFiles.Find(fileID));
	if (pCacheFileName == nullptr) return nullptr;

	return pCacheFileName;
}


//	Clear file cache
void TPackage::ClearFileCache()
{	
	THashNode<int, SharedFile*> * pHead = m_sharedFiles.GetHead();
	THashNode<int, SharedFile*> * pNode = pHead;
	while (pNode) {
		SharedFile* pFileItem = pNode->m_value;
		if (!pFileItem->m_refCnt == 0)
		{
			delete pFileItem->m_pFileData;
			delete pFileItem;
			THashNode<int, SharedFile*> * pSave = pNode;
			pNode = m_sharedFiles.GetNext(pNode);
			m_sharedFiles.Remove(pSave->m_key);
		}
		else {
			pNode = m_sharedFiles.GetNext(pNode);
		}
	}
}

unsigned long TPackage::OpenSharedFile(const char* fileName, unsigned char** ppFileBuf, unsigned long* pFileLen)
{
	//	Get file entry
	FileEntry FileEntry;
	int entryIndex;
	if (!GetFileEntry(fileName, &FileEntry, &entryIndex))
	{
		if (!strstr(fileName, "Textures") && !strstr(fileName, "Tex_"))
		{
			TLog::Log(LOG_ERR, "FILE", "TPackage::OpenSharedFile, Failed to find file [%s] in package", fileName);
		}
		return false;
	}

	TAssert(m_fileEntries[entryIndex]);
	//	Allocate file data buffer
	unsigned char*  pFileData = new unsigned char[FileEntry.m_length];
	if (!pFileData)
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::OpenSharedFile, Not enough memory");
		return false;
	}

	//	Read file data
	unsigned long dwFileLen = FileEntry.m_length;
	if (!ReadFile(FileEntry, pFileData, &dwFileLen))
	{
		delete[] pFileData;
		TLog::Log(LOG_ERR, "FILE", "TPackage::OpenSharedFile, Failed to read file data [%s]", fileName);
		return false;
	}

	//	Add it to shared file arrey
	SharedFile* pFileItem = new SharedFile;
	if (!pFileItem)
	{
		delete[] pFileData;
		TLog::Log(LOG_ERR, "FILE", "TPackage::OpenSharedFile, Not enough memory");
		return false;
	}

	pFileItem->m_cached = false;
	pFileItem->m_tempMem = false;
	pFileItem->m_fileID = 0;
	pFileItem->m_fileLen = dwFileLen;
	pFileItem->m_refCnt = 1;
	pFileItem->m_pFileData = pFileData;
	pFileItem->m_pFileEntry = m_fileEntries[entryIndex];

	//	pFileItem->pFileEntry->iAccessCnt++;
	*ppFileBuf = pFileData;
	*pFileLen = dwFileLen;
	
	//TBD: Why this
	return (int)pFileItem;
}

//	Close a shared file
void TPackage::CloseSharedFile(int fileHandle)
{
	SharedFile* pFileItem = (SharedFile*)fileHandle;
	TAssert(pFileItem && pFileItem->m_refCnt > 0);

	//	No cache file, release it
	delete pFileItem->m_pFileData;
	delete pFileItem;
}


bool TPackage::SetAlgorithmID(int id)
{
	switch (id)
	{
	case 111:
		m_guardByte0 = 0xab12908f;
		m_guardByte1 = 0xb3231902;
		m_maskPasswd = 0x2a63810e;
		m_checkMask = 0x18734563;
		break;

	default:
		m_guardByte0 = 0xfdfdfeee + id * 0x72341f2;
		m_guardByte1 = 0xf00dbeef + id * 0x1237a73;
		m_maskPasswd = 0xa8937462 + id * 0xab2321f;
		m_checkMask = 0x59374231 + id * 0x987a223;
		break;
	}

	return true;
}