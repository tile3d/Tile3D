#include "TSkinModelMan.h"
#include "TSkinModel.h"
#include <Common/TLog.h>
#include <File/TFileImage.h>
#include <File/TFileDir.h>

TSkinModel* TSkinModelMan::LoadSkinModel(const char * SkinModelFile, int skinFlag)
{
	TSkinModel * pSkinModel = FindSkinModelByFile(SkinModelFile);
	if (pSkinModel != nullptr) {
		return pSkinModel->Clone();
	}

	TFileImage fi;
	if (!fi.Open("", SkinModelFile, TFile::TFILE_OPENEXIST | TFile::TFILE_BINARY | TFile::TFILE_TEMPMEMORY)) {
		fi.Close();
		TLog::Log(LOG_ERR, "SkinModelModel", "TSkinModelMan::LoadSkinModel,  failed to load the SkinModel: [%s].", SkinModelFile);
		return nullptr;
	}
	TSkinModel* pNewSkinModel = CreateSkinModel(&fi, skinFlag);
	fi.Close();
	return pNewSkinModel;
}

TSkinModel* TSkinModelMan::LoadSkinModel(TFile * pFile, int skinFlag)
{
	TSkinModel * pSkinModel = FindSkinModelByFile(pFile->GetRelativeFileName());
	if (pSkinModel != nullptr) {
		return pSkinModel->Clone();
	}
	return CreateSkinModel(pFile, skinFlag);
}

bool TSkinModelMan::ReleaseSkinModel(int skinModelID)
{
	TSkinModel * pSkinModel = FindSkinModelByID(skinModelID);
	if (pSkinModel == nullptr) return false;
	pSkinModel->Release();
	return true;
}

bool TSkinModelMan::ReleaseSkinModel(TSkinModel * pSkinModel)
{
	return ReleaseSkinModel(pSkinModel->GetSkinModelID());
}

TSkinModel * TSkinModelMan::FindSkinModelByFile(const char * SkinModelFile)
{
	char relFile[MAX_PATH];
	relFile[0] = '\0';

	TFileDir::GetInstance()->GetRelativePath(SkinModelFile, relFile);
	int SkinModelFileID = TFileDir::GetInstance()->GetIDFromFileName(relFile);
	return FindSkinModelByID(SkinModelFileID);
}

TSkinModel* TSkinModelMan::FindSkinModelByID(int SkinModelFileID)
{
	m_lock.Lock();
	TSkinModel ** pSkinModel = m_skinModels.Find(SkinModelFileID);
	m_lock.Unlock();
	if (pSkinModel == nullptr) {
		return nullptr;
	}
	return (*pSkinModel);
}

TSkinModel * TSkinModelMan::CreateSkinModel(TFile * pFile, int skinFlag)
{
	TSkinModel * pSkinModel = new TSkinModel();

	if (!pSkinModel->Load(pFile, skinFlag)) {
		delete pSkinModel;
		TLog::Log(LOG_ERR, "SkinModelModel", "TSkinModelMan::CreateSkinModel,  Failed to create the SkinModel: [%s].", pFile->GetRelativeFileName());
		return nullptr;
	}

	m_lock.Lock();
	m_skinModels.Put(pSkinModel->GetSkinModelID(), pSkinModel);
	m_lock.Unlock();
	return pSkinModel;
}
