#include "TSkinMeshMan.h"
#include "TSkin.h"
#include "TSkinMesh.h"
#include <File/TFile.h>
#include <File/TFileImage.h>
#include <File/TFileDir.h>
#include <Util/TLog.h>

TSkinMesh* TSkinMeshMan::LoadSkinMesh(const char * fileName, TSkin* pSkin, int skinIndex, int skinNum, int skinVersion)
{
	TSkinMesh * pSkinMesh = FindSkinMeshByFile(fileName);
	if (pSkinMesh != nullptr) {
		return pSkinMesh->Clone();
	}

	TFileImage fi;
	if (fi.Open("", fileName, TFile::TFILE_OPENEXIST | TFile::TFILE_BINARY | TFile::TFILE_TEMPMEMORY)) {
		fi.Close();
		TLog::Log(LOG_ERR, "SkinModel", "TSkinMeshMan::LoadSkinMesh,  Failed to load the skin mesh: [%s].", fileName);
		return nullptr;
	}
	TSkinMesh * pNewSkinMesh = CreateSkinMesh(&fi, pSkin, skinIndex, skinNum, skinVersion);
	fi.Close();
	return pSkinMesh;
}

TSkinMesh* TSkinMeshMan::LoadSkinMesh(TFile * pFile, TSkin* pSkin, int skinIndex, int skinNum, int skinVersion)
{
	TSkinMesh * pSkinMesh = FindSkinMeshByFile(pFile);
	if (pSkinMesh != nullptr) {
		return pSkinMesh->Clone();
	}

	TSkinMesh* pNewSkinMesh = CreateSkinMesh(pFile, pSkin, skinIndex, skinNum, skinVersion);
	return pNewSkinMesh;
}

TSkinMesh* TSkinMeshMan::FindSkinMeshByFile(const char * skinMeshFile)
{
	char relFile[MAX_PATH];
	relFile[0] = '\0';

	TFileDir::GetInstance()->GetRelativePath(skinMeshFile, relFile);
	int skinMeshFileID = TFileDir::GetInstance()->GetIDFromFileName(relFile);
	return FindSkinMeshByID(skinMeshFileID);
}

TSkinMesh* TSkinMeshMan::FindSkinMeshByFile(TFile * pFile)
{
	return FindSkinMeshByFile(pFile->GetRelativeFileName());
}


TSkinMesh* TSkinMeshMan::FindSkinMeshByID(int skinMeshFileID)
{
	m_lock.Lock();
	TSkinMesh ** pSkinMesh = m_skinMeshs.Find(skinMeshFileID);
	m_lock.Unlock();
	if (pSkinMesh == nullptr) {
		return nullptr;
	}
	return *pSkinMesh;
}

TSkinMesh * TSkinMeshMan::CreateSkinMesh(TFile * pFile, TSkin* pSkin, int skinIndex, int skinNum, int skinVersion)
{
	TSkinMesh * pSkinMesh = new TSkinMesh();

	if (!pSkinMesh->Load(pFile, pSkin, skinIndex, skinNum, skinVersion)) {
		delete pSkinMesh;
		TLog::Log(LOG_ERR, "SkinModel", "TSkinMeshMan::CreateSkinMesh,  Failed to load the skin mesh: [%s].", pFile->GetRelativeFileName());
		return nullptr;
	}

	m_lock.Lock();
	m_skinMeshs.Put(pSkinMesh->GetSkinMeshID(), pSkinMesh);
	m_lock.Unlock();
	return pSkinMesh;
}
