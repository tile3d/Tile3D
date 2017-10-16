#include "TSkinMan.h"
#include "TSkin.h"
#include <File/TFileImage.h>
#include <File/TFileDir.h>
#include <Common/TLog.h>

TSkin* TSkinMan::LoadSkin(const char * skinFile)
{
	TSkin * pSkin = FindSkinByFile(skinFile);
	if (pSkin != nullptr) {
		return pSkin->Clone();
	}

	TFileImage fi;
	if (!fi.Open("", skinFile, TFile::TFILE_OPENEXIST | TFile::TFILE_BINARY | TFile::TFILE_TEMPMEMORY)) {
		fi.Close();
		TLog::Log(LOG_ERR, "SkinModel", "TSkinMan::LoadSkin,  Failed to load the Skin: [%s].", skinFile);
		return nullptr;
	}
	TSkin* pNewSkin = CreateSkin(&fi);
	fi.Close();
	return pNewSkin;
}

TSkin* TSkinMan::LoadSkin(TFile * pFile)
{
	TSkin * pSkin = FindSkinByFile(pFile->GetRelativeFileName());
	if (pSkin != nullptr) {
		return pSkin->Clone();
	}
	return CreateSkin(pFile);
}

TSkin* TSkinMan::FindSkinByFile(const char * skinFile)
{
	char relFile[MAX_PATH];
	relFile[0] = '\0';

	TFileDir::GetInstance()->GetRelativePath(skinFile, relFile);
	int skinFileID = TFileDir::GetInstance()->GetIDFromFileName(relFile);
	return FindSkinByID(skinFileID);
}

TSkin* TSkinMan::FindSkinByID(int skinFileID)
{
	m_lock.Lock();
	TSkin ** pSkin = m_skins.Find(skinFileID);
	m_lock.Unlock();
	if (pSkin == nullptr) {
		return nullptr;
	}
	return *pSkin;
}

TSkin * TSkinMan::CreateSkin(TFile * pFile)
{
	TSkin * pSkin = new TSkin();

	if (!pSkin->Load(pFile)) {
		delete pSkin;
		TLog::Log(LOG_ERR, "SkinModel", "TSkinMan::CreateSkin,  Failed to create the Skin: [%s].", pFile->GetRelativeFileName());
		return nullptr;
	}

	m_lock.Lock();
	m_skins.Put(pSkin->GetSkinID(), pSkin);
	m_lock.Unlock();
	return pSkin;
}
