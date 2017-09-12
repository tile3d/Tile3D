#include "TSkinModel.h"
#include "TSkeleton.h"
#include "TSkeletonMan.h"
#include "TSkinMan.h"
#include <Util/TLog.h>
#include <File/TFile.h>
#include <File/TFileImage.h>
#include <File/TFileDir.h>

TSkinModel::TSkinModel()
{
	m_modelID = 0;
	m_version = 0;
	m_numSkin = 0;
	m_numAction = 0;
	m_numHanger = 0;
	m_numProp = 0;
	m_pSkeleton = nullptr;
}


TSkinModel::~TSkinModel()
{

}

TSkinModel* TSkinModel::Clone()
{
	return this;
}

bool TSkinModel::Load(const char * pFile, int skinFlag)
{
	TFileImage file;
	if (!file.Open("", pFile, TFile::TFILE_OPENEXIST | TFile::TFILE_BINARY | TFile::TFILE_TEMPMEMORY)) {
		file.Close();
		TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  Can't open file [%s].", pFile);
		return false;
	}

	if (!Load(&file, skinFlag)) {
		file.Close();
		TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  Failed to load the file [%s].", pFile);
		return false;
	}

	file.Close();
	return true;
}


bool TSkinModel::Load(TFile * pFile, int skinFlag)
{
	TSkinModelHeader header;
	int readCnt = 0;

	if (!(pFile->Read(&header, sizeof(header), &readCnt)) || readCnt != sizeof(header)) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  Failed to read the file header [%s].", pFile->GetFileName());
		return false;
	}

	if (header.m_flags != SKINMODEL_IDENTIFY || header.m_version != SKINMODEL_VERSION) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  Invalid file identify or version [%s].", pFile->GetFileName());
		return false;
	}

	m_fileName = pFile->GetRelativeFileName();
	m_modelID = TFileDir::GetInstance()->GetIDFromFileName(m_fileName);
	m_version = header.m_version;
	m_numSkin = header.m_numSkin;
	m_numAction = header.m_numAction;
	m_numHanger = header.m_numHanger;
	m_numProp = header.m_numProp;

	TString bonFile, filePath;
	TFileDir::GetInstance()->GetFilePath(pFile->GetRelativeFileName(), filePath);
	pFile->ReadString(bonFile);
	if (bonFile.GetLength() > 0) {
		if (filePath.GetLength() > 0) {
			bonFile = filePath + "\\" + bonFile;
		}
	}

	if (bonFile.GetLength() <= 0) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  Invalid bon file [%s].", pFile->GetFileName());
		return false;
	}

	//Load the skeleton
	if (LoadSkeleton(bonFile)) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  failed to load the skeleton, skeleton file=%s", bonFile);
		return false;
	}

	//Load the skins
	TString skinFile;
	for (int i = 0; i < header.m_numSkin; i++) {
		pFile->ReadString(skinFile);
		if (skinFlag == SKIN_LOAD_NOSKIN) {
			continue;
		}

		if (skinFile.GetLength() > 0) {
			if (filePath.GetLength() > 0) {
				skinFile = filePath + "\\" + bonFile;
			}
		}

		if (skinFile.GetLength() <= 0) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  Invalid skin file [%s].", pFile->GetFileName());
			return false;
		}

		if (skinFlag == SKIN_LOAD_DEFAULT) {
			if (!LoadSkin(skinFile, false)) {
				TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  fail to load the skin file [%s].", skinFile);
			}
		}
		else if (skinFlag == SKIN_LOAD_UNIQUESKIN) {
			if (!LoadSkin(skinFile, true)) {
				TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  fail to load the unique skin file [%s].", skinFile);
			}
		}
	}

	return true;
}


TSkeleton* TSkinModel::LoadSkeleton(const char * skeletonFile)
{
	TSkeleton * pSkeleton = TSkeletonMan::GetInstance()->LoadSkeleton(skeletonFile);
	if (pSkeleton == nullptr) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::LoadSkeleton,  failed to load the skeleton, skeleton file=%s", skeletonFile);
		return false;
	}

	m_pSkeleton = pSkeleton;
	m_pSkeleton->SetSkinModel(this);

	return pSkeleton;
}

TSkin * TSkinModel::LoadSkin(const char* skinFile, bool autoFree)
{
	TSkin * pSkin = TSkinMan::GetInstance()->LoadSkin(skinFile);
	if (pSkin == nullptr) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::LoadSkin,  failed to load the skin, skin file=%s", skinFile);
		return false;
	}


	return pSkin;
}
