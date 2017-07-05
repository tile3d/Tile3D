#include "TFile.h"

TFile::TFile()
{
	m_pFile = nullptr;
	m_fullPathFileName[0] = '\0';
	m_relativeFileName[0] = '\0';
	m_flags = 0;
	m_timestamp = 0;
	m_hasOpen = false;
}

TFile::~TFile()
{
	Close();
}


void TFile::Close()
{
	if (m_pFile) {
		fclose(m_pFile);
		m_pFile = nullptr;
	}
	m_hasOpen = false;
}

bool TFile::Open(const char* folderName, const char* fileName, int flags)
{
	char fullPathFileName[MAX_PATH];
	
	return true;
}

