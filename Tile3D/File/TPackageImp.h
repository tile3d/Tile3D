#pragma once

#include <Container/TString.h>
#include <Container/TArray.h>
#include <Util/TTypes.h>

class TPackageFile;
class TPackageImp
{
public:
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
	TPackageImp();
	virtual ~TPackageImp();

	bool Open(const char * pckPath, OPEN_MODE mode, bool encrypt);
	bool Open(const char * pckPath, const char *folder, OPEN_MODE mode, bool encrytp);
	virtual bool Close();



private:
	bool m_changed;
	bool m_readonly;
	bool m_useShortName;

	TPackageFile * m_pPackageFile;
	char	m_pckFileName[MAX_PATH];
	char	m_folder[MAX_PATH];

	int m_cacheSize;
	int m_sharedSize;

	bool m_hasSaferHeader;
	SafeFileHeader m_safeHeader;

	TArray<FileEntry*>	m_fileEntries;
	TArray<FileEntryCache*> m_fileEntryCaches;

};


