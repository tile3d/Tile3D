#include "TPackage.h"
#include "TPackageFile.h"
#include "TFileDir.h"
#include "Util/TLog.h"
#include "Util/TAssert.h"
#include "Core/TMemory.h"

TPackage::PackageDir::~PackageDir()
{
	Clear();
}


int TPackage::PackageDir::SearchItemIndex(const char * name, int * pos)
{
	int left, right, mid;
	left = 0;
	right = m_list.Size() - 1;
	mid = 0;
	while (left <= right)
	{
		mid = (left + right) / 2;
		int rst = stricmp(name, m_list[mid]->_name);
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
		//Ãû×ÖÖØ¸´
		return -1;
	}

	if (pos >= (int)(m_list.Size()))
	{
		m_list.Add(item);
	}
	else
	{
		int rst = stricmp(item->_name, m_list[pos]->_name);
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


int
TPackage::PackageDir::Clear()
{
	size_t i;
	for (i = 0; i< m_list.Size(); i++)
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
	m_pPackageFile = nullptr;
	m_sharedSize = 0;
	m_cacheSize = 0;

	m_pckFileName[0] = '\0';
	m_bHasSaferHeader = false;
}

TPackage::~TPackage()
{
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

	return InnerOpen(pckPath, folder, bEncrypt, false);
}

bool TPackage::Open(const char* pckPath, const char* folder,  bool bEncrypt)
{
	return InnerOpen(pckPath, folder, bEncrypt, true);
}

bool TPackage::InnerOpen(const char* pckPath, const char* folder,  bool bEncrypt, bool bShortName)
{
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
		strlwr(m_folder);
		NormalizeFileName(m_folder);

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
			m_pPackageFile = NULL;

			TLog::Log(LOG_ERR, "FILE", "TPackage::InnerOpen, Can not open file [%s]", fullPckPath);
			return false;
		}
		m_bReadOnly = true;
	}

	strncpy(m_pckFileName, pckPath, MAX_PATH);

	LoadSafeHeader();

	int offset;
	m_pPackageFile->Seek(0, SEEK_END);
	offset = m_pPackageFile->Tell();
	m_pPackageFile->Seek(0, SEEK_SET);

	if (m_bHasSaferHeader)
		offset = (int)m_safeHeader.offset;

	// Now analyse the file entries of the package;
	int version;
	// First version;
	m_pPackageFile->Seek(offset - sizeof(int), SEEK_SET);
	m_pPackageFile->Seek(&version, sizeof(int), 1);

	if (version == 0x00020002 || version == 0x00020001)
	{
		int i, numFile;

		// Now read file number;
		m_pPackageFile->Seek(offset - (sizeof(int) + sizeof(int)), SEEK_SET);
		m_pPackageFile->Read(&numFile, sizeof(int), 1);
		m_pPackageFile->Seek(offset - (sizeof(FileHeader) + sizeof(int) + sizeof(int)), SEEK_SET);
		m_pPackageFile->Read(&m_header, sizeof(FileHeader), 1);
		if (strstr(m_header.m_description, "lica File Package") == NULL)
			return false;
		strncpy(m_header.m_description, AFPCK_COPYRIGHT_TAG, sizeof(m_header.m_description));

		// if we don't expect one encrypt package, we will let the error come out.
		// make sure the encrypt flag is correct
		bool bPackIsEncrypt = (m_header.m_flags & PACKFLAG_ENCRYPT) != 0;
		if (bEncrypt != bPackIsEncrypt)
		{
			TLog::Log(LOG_ERR, "FILE", "TPackage::InnerOpen, wrong encrypt flag");
			return false;
		}

		m_header.m_entryOffset ^= AFPCK_MASKDWORD;

		if (m_header.m_guardByte0 != AFPCK_GUARDBYTE0 ||
			m_header.m_guardByte1 != AFPCK_GUARDBYTE1)
		{
			// corrput file
			TLog::Log(LOG_ERR, "FILE", "TPackage::InnerOpen, GuardBytes corrupted [%s]", pckPath));
			return false;
		}

		//	Seek to entry list;
		m_pPackageFile->Seek(m_header.m_entryOffset, SEEK_SET);

		//	Create entries
		m_fileEntries.SetSize(numFile, 100);
		m_fileEntryCaches.SetSize(numFile, 100);

		for (i = 0; i < numFile; i++)
		{
			FileEntry* pEntry = new FileEntry;
			pEntry->m_accessCnt = 0;

			FileEntryCache* pEntryCache = new FileEntryCache;
			memset(pEntryCache, 0, sizeof(FileEntryCache));

			// first read the entry size after compressed
			int nCompressedSize;
			m_pPackageFile->Read(&nCompressedSize, sizeof(int), 1);
			nCompressedSize ^= AFPCK_MASKDWORD;

			int checkSize;
			m_pPackageFile->Read(&checkSize, sizeof(int), 1);
			checkSize = checkSize ^ AFPCK_CHECKMASK ^ AFPCK_MASKDWORD;

			if (nCompressedSize != checkSize)
			{
				TLog::Log(LOG_ERR, "FILE", "TPackage::InnerOpen, Check Byte Error!");
				return false;
			}

			pEntryCache->m_compressedLength = nCompressedSize;
			pEntryCache->m_pEntryCompressed = (char*)TMemory::Alloc(nCompressedSize);

			m_pPackageFile->Read(pEntryCache->m_pEntryCompressed, nCompressedSize, 1);
			int entrySize = sizeof(FileEntry);

			if (entrySize == nCompressedSize)
			{
				memcpy(pEntry, pEntryCache->m_pEntryCompressed, sizeof(FileEntry));

				// maybe the original package fileentry has not been compressed
				int compressedSize = sizeof(FileEntry);
				char * pBuffer = (char *)TMemory::Alloc(sizeof(FileEntry));
				int nRet = Compress((char*)pEntry, sizeof(FileEntry), pBuffer, &compressedSize);
				if (nRet != 0 || compressedSize >= sizeof(FileEntry))
				{
					compressedSize = sizeof(FileEntry);
					memcpy(pBuffer, pEntry, sizeof(FileEntry));
				}
				pEntryCache->m_compressedLength = compressedSize;
				pEntryCache->m_pEntryCompressed = (char *)TMemory::Realloc(pEntryCache->m_pEntryCompressed, compressedSize);
				memcpy(pEntryCache->m_pEntryCompressed, pBuffer, compressedSize);
				TMemory::Free(pBuffer);
			}
			else
			{
				if (0 != Uncompress(pEntryCache->m_pEntryCompressed, nCompressedSize, (char*)pEntry, &entrySize))
				{
					TLog::Log(LOG_ERR, "FILE", "TPackage::InnerOpen,decode file entry fail!");
					return false;
				}

				TAssert(entrySize == sizeof(FileEntry));
			}

			//	Note: A bug existed in AppendFileCompressed() after m_bUseShortName was introduced. The bug
			//		didn't normalize file name when new file is added to package, so that the szFileName of
			//		FILEENTRY may contain '/' character. The bug wasn't fixed until 2013.3.18, many 'new' files
			//		have been added to package, so NormalizeFileName is inserted here to ensure all szFileName
			//		of FILEENTRY uses '\' instead of '/', at least in memory.
			NormalizeFileName(pEntry->m_fileName, false);

			m_fileEntries[i] = pEntry;
			m_fileEntryCaches[i] = pEntryCache;
		}

		ResortEntries();

		// now we move entry point to the end of the file so to keep old entries here
		if (m_bHasSaferHeader)
			m_header.m_entryOffset = offset;
	}
	else
	{
		TLog::Log(LOG_ERR, "FILE", "TPackage::InnerOpen,Incorrect version!"));
		return false;
	}


	m_mode = OPEN_EXIST;
	m_bChanged = false;
	m_sharedSize = 0;
	m_cacheSize = 0;
	return true;
}

