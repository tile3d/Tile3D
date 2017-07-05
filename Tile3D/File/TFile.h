#pragma once

#include <stdio.h>
#include <Util/TTypes.h>
#include <Core/TObject.h>

class TFile : public TObject
{
public:
	TFile();
	virtual ~TFile();

	bool Open(const char* folderName, const char* fileName, int flags);

private:
	void Close();

private:
	FILE * m_pFile;
	char m_fullPathFileName[MAX_PATH];		//fullpath file name
	char m_relativeFileName[MAX_PATH];		//relative file name
	int m_flags;
	int m_timestamp;
	bool m_hasOpen;
};

