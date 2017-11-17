#include <Common/TLog.h>
#include <File/TFile.h>
#include <File/TFileImage.h>
#include <File/TFileDir.h>
#include "TSkinModel.h"
#include "TSkeleton.h"
#include "TSkin.h"
#include "TSkinModelAction.h"
#include "TSkinModelMan.h"
#include "TSkeletonBone.h"

TSkinModel::TSkinModel()
{
	Init();
}

void TSkinModel::Init()
{
	m_modelID = 0;
	m_version = 0;
	m_numSkin = 0;
	m_numAction = 0;
	m_numHanger = 0;
	m_numProp = 0;
	m_refcount = 1;
	m_pSkeleton = nullptr;

}

TSkinModel::~TSkinModel()
{
	if (m_pSkeleton) {
		delete m_pSkeleton;
	}

	for (int i = 0; i < m_skins.Size(); i++) {
		delete m_skins[i];
	}

	for (int i = 0; i < m_actions.Size(); i++) {
		delete m_actions[i];
	}

}

void TSkinModel::Release()
{
	--m_refcount;
	if (m_refcount <= 0) {
		delete this;
	}
}

TSkinModel* TSkinModel::Clone()
{
	++m_refcount;
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
	if (!LoadSkeleton(bonFile)) {
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
				skinFile = filePath + "\\" + skinFile;
			}
		}

		if (skinFile.GetLength() <= 0) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  Invalid skin file [%s].", pFile->GetFileName());
			return false;
		}

		TSkin * pSkin = nullptr;
		if (skinFlag == SKIN_LOAD_DEFAULT) {
			if ((pSkin = LoadSkin(skinFile, false)) == nullptr) {
				TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  fail to load the skin file [%s].", skinFile);
			}
		}
		else if (skinFlag == SKIN_LOAD_UNIQUESKIN) {
			if ((pSkin = LoadSkin(skinFile, true)) == nullptr) {
				TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  fail to load the unique skin file [%s].", skinFile);
			}
		}
		m_skins.Add(pSkin);
	}


	//Load the physique
	TString phyFile;
	pFile->ReadString(phyFile);
	m_phyFileName = phyFile;

	//TBD
	if (header.m_version < 8) {

	}
	else {
		TString tckDirName;
		pFile->ReadString(tckDirName);
		m_tckDirName = tckDirName;
	}

	TString tckPathName;
	if (m_tckDirName.GetLength() > 0 && filePath.GetLength() > 0) {
		tckPathName.Format("%s\\%s", filePath, m_tckDirName);
	}
	else if (filePath.GetLength() > 0) {
		tckPathName = filePath;
	}
	else {
		tckPathName = m_tckDirName;
	}

	//Load all the action frame data
	for (int i = 0; i < header.m_numAction; i++) {
		TSkinModelAction * pAction = new TSkinModelAction;
		if (!pAction->Load(pFile, header.m_version, tckPathName)) {
			delete pAction;
			return false;
		}

		if (header.m_version < 7) {
			pAction->SetTrackSetFileName(m_pSkeleton->GetFileName());
		}
		m_actions.Add(pAction);
	}


	//Load all the hangers(TBD)
	for (int i = 0; i < header.m_numHanger; i++) {
		LoadHanger(pFile);
	}

	//Load all the propertry
	TString propName;
	TString propVal;
	for (int i = 0; i < header.m_numProp; i++) {
		pFile->ReadString(propName);
		pFile->ReadString(propVal);
		m_props.Put(propName, propVal);
	}
	return true;
}


TSkeleton* TSkinModel::LoadSkeleton(const char * skeletonFile)
{
	TSkeleton * pSkeleton = new TSkeleton();
	if (!pSkeleton->Load(skeletonFile)) {
		delete pSkeleton;
		TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::LoadSkeleton,  failed to load the skeleton, skeleton file=%s", skeletonFile);
		return nullptr;
	}

	m_pSkeleton = pSkeleton;
	m_pSkeleton->SetSkinModel(this);
	return pSkeleton;
}

TSkin * TSkinModel::LoadSkin(const char* skinFile, bool autoFree)
{
	TSkin * pSkin = new TSkin();
	if (!pSkin->Load(skinFile)) {
		delete pSkin;
		TLog::Log(LOG_ERR, "SkinModel", "TSkinMan::CreateSkin,  Failed to create the Skin: [%s].", skinFile);
		return nullptr;
	}
	return pSkin;
}

THanger * TSkinModel::LoadHanger(TFile * pFile)
{
	return nullptr;
}

void TSkinModel::Render()
{
	for (int i = 0; i < m_skins.Size(); ++i) {
		TSkin* pSkin = m_skins[i];
		pSkin->Render(this);
	}
}

bool TSkinModel::UpdateBlendMatrixs()
{
	int boneNum = m_pSkeleton->GetBoneNum();
	for (int i = 0; i < boneNum; i++) {
		TSkeletonBone* pBone = m_pSkeleton->GetBone(i);
		m_blendMats[i] = pBone->GetBoneInitTM();
	}
	return true;
}

