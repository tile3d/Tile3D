#pragma once

#include <Container/TString.h>
#include <Container/TArray.h>
#include <Container/THashMap.h>
#include <Util/TTypes.h>
#include <Core/Lock/TMutexLock.h>
#include "TPackageFile.h"


//	#define AFPCK_VERSION	0x00010001
//	#define AFPCK_VERSION	0x00010002	//	Add compression
//  #define AFPCK_VERSION	0x00010003	//	The final release version on June 2002
//	#define AFPCK_VERSION	0x00020001	//	The version for element before Oct 2005
#define PCK_VERSION		0x00020003	//	The version with safe header
#define PCK_COPYRIGHT_TAG "Angelica File Package, Perfect World Co. Ltd. 2002~2008. All Rights Reserved. "
#define PCK_MAX_FILE_SIZE	0x7fffff00U


//
//TBD
//1) Resolve the pck file size problem(2G problem), Support single large pck file (for example 8G) 
//2) Use the same memory allocation and deallocation method (new vs TMemory::Alloc) [DONE]
//3) share file not used now, investigate if can open it
//4) implement the unpack logic(consider multithread)
//


//
//
//Package format
//[SafeHeader][File Content][File Content][...][FileEntry][FileEntry][....][PackageHeader][NumFiles][Version]
//
//Suppport operations
//1) Append: append the file content in the end of file content
//2) Remove: remove the entry from the file entry list, file content not modified(memory fragment)
//3) Replace:add the file content in the end of file content
//
class TPackage
{
public:
	enum PCK_FLAG
	{
		PCK_FLAG_ENCRYPT = 0x80000000
	};


	class PackageEntry
	{
	public:
		char *_name;
		PackageEntry() :_name(NULL) {}
		PackageEntry(const char * name) { _name = new char[strlen(name) + 1]; strcpy(_name, name); }
		virtual ~PackageEntry() { if (_name) delete[]_name; }
		virtual bool IsContainer() = 0;
		virtual int GetIndex() = 0;
		virtual PackageEntry * SearchItem(const char *) = 0;
	};

	class PackageDir : public PackageEntry
	{
		TArray<PackageEntry *> m_list;
		int SearchItemIndex(const char * name, int * pos);
	public:
		PackageDir(const char * name) :PackageEntry(name) {}
		PackageDir() {}
		~PackageDir(){ Clear(); }
		int Clear();
		virtual bool IsContainer() { return true; }
		virtual int GetIndex() { return -1; }
		virtual PackageEntry * SearchItem(const char *);
	public:
		PackageEntry * GetItem(int index);
		int GetEntryCount() { return m_list.Size(); }
		int RemoveItem(const char * name);
		int AppendEntry(PackageEntry *);
		int SearchEntry(const char * filename);
	};

	class PackageFile : public PackageEntry
	{
		int m_index;
	public:
		PackageFile(const char * name, int index) :PackageEntry(name), m_index(index) {}
		virtual bool IsContainer() { return false; }
		virtual PackageEntry * SearchItem(const char *) { return NULL; }
		virtual int GetIndex() { return m_index; }
		void SetIndex(int index) { m_index = index; }
	};

	//SafePackageHeader info in the file beginning
	struct SafePackageHeader
	{
		int	m_tag;			//	tag of safe header, current it is 0x4DCA23EF
		int64 m_offset;		//	offset of real entries
	};

	//old package info(only support 2G single file)
	struct SafePackageHeaderOld
	{
		int	m_tag1;			//	tag of safe header, current it is 0x4DCA23EF
		int m_offset;		//	offset of real entries
		int m_tag2;			// 0x56a089b7
	};

	//PackageHeader info in the file end
	struct PackageHeader
	{
		int	m_guardByte0;			//	0xabcdefab
		int	m_version;				//	Composed by two word version, major part and minor part;
		int64 m_entryOffset;			//	The entry list offset from the beginning;
		int	m_flags;				//	package flags. the highest bit means the encrypt state;
		char m_description[252];		//	Description
		int	m_guardByte1;				//	0xffeeffee
	};

	struct FileEntry
	{
		char	m_fileName[MAX_PATH];	//	The file name of this entry; this may contain a path;
		int64	m_offset;				//	The offset from the beginning of the package file;
		unsigned long	m_length;		//	The length of this file;
		unsigned long	m_compressedLength;		//	The compressed data length;
		int		m_accessCnt;			//	Access counter used by OpenSharedFile	
	};

	struct FileEntryCache
	{
		int m_compressedLength;
		char * m_pEntryCompressed;
	};

	//	Share read file item
	struct SharedFile
	{
		int		m_fileID;		//	File ID
		bool	m_cached;		//	Cached flag
		bool	m_tempMem;		//	true, use temporary memory alloctor
		int		m_refCnt;		//	Reference counter
		unsigned char*	m_pFileData;		//	File data buffer
		int		m_fileLen;		//	File data length
		FileEntry*	m_pFileEntry;	//	Point to file entry
	};

	//	Cache file name
	struct CacheFilename
	{
		TString	m_fileName;	//	File name
		int	m_fileID;		//	File ID
	};



public:
	TPackage();
	virtual ~TPackage();


	bool Open(const char* pckPath, bool bEncrypt);
	bool Open(const char* pckPath, const char* folder, bool bEncrypt, bool bShortName);
	bool Create(const char* pckPath, const char *folder, bool bEncrypt);
	bool Close();

	// Find a file entry
	bool GetFileEntry(const char* szFileName, FileEntry* pFileEntry, int* pIndex);
	const FileEntry* GetFileEntryByIndex(int index) const { return m_fileEntries[index]; }
	bool CheckFileEntryValid(FileEntry* pFileEntry);

	PackageDir* GetDirEntry(const char* path);

	//	Append a file into directroy
	bool InsertFileToDir(const char * filename, int index);
	// Remove File from directory
	bool RemoveFileFromDir(const char * filename);

	virtual bool IsFileExist(const char* szFileName);

	bool AppendFile(const char* fileName, unsigned char* pFileBuffer, unsigned long fileLength, bool bCompress);
	bool AppendFileCompressed(const char * fileName, unsigned char* pCompressedBuffer, unsigned long fileLength, unsigned long compressedLength);

	//Remove a file from the package, only remove the file entry from the package, the file's data will remain in the package
	bool RemoveFile(const char* szFileName);

	//Replace a file in the package, we will only replace the file entry in the package, the old file's data will remain in the package
	bool ReplaceFile(const char* szFileName, unsigned char* pFileBuffer, unsigned long fileLength, bool bCompress);
	bool ReplaceFileCompressed(const char * szFileName, unsigned char* pCompressedBuffer, unsigned long fileLength, unsigned long compressedLength);

	bool ReadFile(const char* szFileName, unsigned char* pFileBuffer, unsigned long* pbufferLen);
	bool ReadFile(FileEntry& fileEntry, unsigned char* pFileBuffer, unsigned long* pbufferLen);

	bool ReadCompressedFile(const char* fileName, unsigned char* pCompressedBuffer, unsigned long* pBufferLen);
	bool ReadCompressedFile(FileEntry& fileEntry, unsigned char* pCompressedBuffer, unsigned long * pdwBufferLen);

	//	Open a shared file
	virtual unsigned long OpenSharedFile(const char* fileName, unsigned char** ppFileBuf, unsigned long* pFileLen);
	//	Close a shared file
	virtual void CloseSharedFile(int fileHandle);

	//	Get current cached file total size
	int GetCachedFileSize() const { return m_cacheCount; }
	//	Get current shared file total size
	int GetSharedFileSize() const { return m_sharedCount; }

	int GetFileNumber() const { return m_fileEntries.Size(); }
	PackageHeader * GetFileHeader() { return &m_header; }
	virtual const char * GetFolder() { return m_folder; }
	const char* GetPckFileName() { return m_pckFileName; }
	int64 GetPackageFileSize() { return m_pPackageFile->GetPackageFileSize(); }


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
	int Compress(unsigned char* pFileBuffer, int fileLength, unsigned char* pCompressedBuffer, unsigned long * pCompressedLength);

	/*
	Uncompress a data buffer
	pCompressedBuffer	IN		buffer contains compressed data to be uncompressed
	compressedLength	IN		the compressed data size
	pFileBuffer			OUT		the uncompressed data buffer
	pFileLength			IN/OUT	the uncompressed data buffer size as input
	when out, it is the real uncompressed data length

	RETURN: 0,		ok
	-1,		dest buffer is too small
	-2,		unknown error
	*/
	int Uncompress(unsigned char* pCompressedBuffer, int compressedLength, unsigned char* pFileBuffer, unsigned long * pFileLength);


	void Encrypt(unsigned char* pBuffer, unsigned long length);
	void Decrypt(unsigned char* pBuffer, unsigned long length);

private:
	//	Safe header
	bool LoadSafeHeader();
	bool SaveSafeHeader();
	bool CreateSafeHeader();

	// Sort the file entry list;
	bool ResortEntries();
	//	Save file entries
	bool SaveEntries(int * pEntrySize);


	//	Add a file name to file cache list
	bool AddCacheFileName(const char* fileName);
	//	Add a group of file names to file cache list
	bool AddCacheFileNameList(const char* descFile);
	//	Search a cache file name from table
	CacheFilename* SearchCacheFileName(const char* fileName);
	CacheFilename* SearchCacheFileName(int fileID);
	void ClearFileCache();

	//	Set algorithm id
	bool SetAlgorithmID(int id);

	int GetGuardByte0() { return m_guardByte0; }
	int GetGuardByte1() { return m_guardByte1; }
	int GetMaskPasswd() { return m_maskPasswd; }
	int GetCheckMask() { return m_checkMask; }

private:
	bool m_bChanged;
	bool m_bReadOnly;
	bool m_bUseShortName;

	int	m_guardByte0;
	int	m_guardByte1;
	int m_maskPasswd;
	int	m_checkMask;


	PackageHeader	m_header;
	PackageDir	m_directory;	//	the ROOT of directory tree. 
	TPackageFile * m_pPackageFile;
	char	m_pckFileName[MAX_PATH];
	char	m_folder[MAX_PATH];

	int m_totalCount;
	int m_cacheCount;
	int m_sharedCount;

	bool m_bHasSaferHeader;
	SafePackageHeader m_safeHeader;

	TArray<FileEntry*>	m_fileEntries;
	TArray<FileEntryCache*> m_fileEntryCaches;

	THashMap<int, CacheFilename*> m_cachedFiles;
	THashMap<int, SharedFile*> m_sharedFiles;

	TMutexLock m_readFileLock;
};



