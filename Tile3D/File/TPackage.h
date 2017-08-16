#pragma once

#include <Container/TString.h>
#include <Container/TArray.h>
#include <Container/THashMap.h>
#include <Util/TTypes.h>




class TPackageFile;
class TPackage
{
public:
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
		~PackageDir();
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

	enum OPEN_MODE
	{
		OPEN_EXIST = 0,
		CREATE_NEW = 1
	};

	struct FileEntry
	{
		char	m_fileName[MAX_PATH];	//	The file name of this entry; this may contain a path;
		int64	m_offset;				//	The offset from the beginning of the package file;
		int		m_length;				//	The length of this file;
		int		m_compressedLength;		//	The compressed data length;
		int		m_accessCnt;			//	Access counter used by OpenSharedFile	
	};

	struct FileEntryCache
	{
		int m_compressedLength;
		char * m_pEntryCompressed;
	};

	struct FileHeader
	{
		int	m_guardByte0;				//	0xabcdefab
		int	m_version;				//	Composed by two word version, major part and minor part;
		int64 m_entryOffset;			//	The entry list offset from the beginning;
		int	m_flags;				//	package flags. the highest bit means the encrypt state;
		char m_description[252];		//	Description
		int	m_guardByte1;				//	0xffeeffee
	};

	//	Share read file item
	struct SharedFile
	{
		int		m_fileID;		//	File ID
		bool	m_cached;		//	Cached flag
		bool	m_tempMem;		//	true, use temporary memory alloctor
		int		m_refCnt;		//	Reference counter
		char*	m_pFileData;		//	File data buffer
		int		m_fileLen;		//	File data length
		FileEntry*	m_pFileEntry;	//	Point to file entry
	};

	//	Cache file name
	struct CacheFilename
	{
		TString	fileName;	//	File name
		int	fileID;		//	File ID
	};

#pragma pack(push)
#pragma pack(4)
	struct SafeFileHeader
	{	
		int	tag;			//	tag of safe header, current it is 0x4DCA23EF
		int64 offset;		//	offset of real entries
	};
#pragma pack(pop)


public:
	TPackage();
	virtual ~TPackage();

	bool Open(const char* pckPath, bool bEncrypt);
	bool Open(const char* pckPath, const char *folder, bool bEncrypt);
	virtual bool Close();

private:
	bool InnerOpen(const char* pckPath, const char* folder, bool bEncrypt, bool bShortName);

private:
	bool m_bChanged;
	bool m_bReadOnly;
	bool m_bUseShortName;

	FileHeader	m_header;
	OPEN_MODE	m_mode;

	PackageDir	m_directory;	//	the ROOT of directory tree. 

	TPackageFile * m_pPackageFile;
	char	m_pckFileName[MAX_PATH];
	char	m_folder[MAX_PATH];

	int m_cacheSize;
	int m_sharedSize;

	bool m_bHasSaferHeader;
	SafeFileHeader m_safeHeader;

	TArray<FileEntry*>	m_fileEntries;
	TArray<FileEntryCache*> m_fileEntryCaches;

	THashMap<int, CacheFilename> m_cachedFiles;
	THashMap<int, SharedFile> m_sharedFiles;
};



