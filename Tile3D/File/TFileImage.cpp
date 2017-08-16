#include "TFileImage.h"
#include "TFileDir.h"
#include "TPackageMan.h"

TFileImage::TFileImage() : TFile()
{
	m_pPackage = nullptr;
	m_pFileImage = nullptr;
	m_nCurPtr = 0;
	m_fileLength = 0;
	m_handle = 0;
	m_bTempMem = false;
}

TFileImage::~TFileImage()
{
	Close();
}

bool TFileImage::Init(const char * fullName, bool bTempMem)
{
	m_bTempMem = bTempMem;
	strncpy(m_fullPathFileName, fullName, MAX_PATH);

	TFileDir::GetInstance()->GetRelativePath(fullName, m_relativeFileName);
	m_pPackage = TPackageMan::GetInstance()->GetPackage(m_relativeFileName);

}

