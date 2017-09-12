#pragma once

#include <Container/THashMap.h>
#include <Core/Lock/TMutexLock.h>

class TFile;
class TSkin;
class TSkinMesh;
class TSkinMeshMan
{
public:
	static TSkinMeshMan * GetInstance() {
		static TSkinMeshMan skinmeshman;
		return &skinmeshman;
	}

	TSkinMesh* LoadSkinMesh(const char * fileName, TSkin* pSkin, int skinIndex, int skinNum, int skinVersion);
	TSkinMesh* LoadSkinMesh(TFile * pFile, TSkin* pSkin, int skinIndex, int skinNum, int skinVersion);

private:
	TSkinMesh* FindSkinMeshByFile(const char * skinFile);
	TSkinMesh* FindSkinMeshByFile(TFile * skinFile);
	TSkinMesh* FindSkinMeshByID(int skeletonFileID);
	TSkinMesh * CreateSkinMesh(TFile * pFile, TSkin* pSkin, int skinIndex, int skinNum, int skinVersion);

private:
	THashMap<int, TSkinMesh*> m_skinMeshs;
	TMutexLock m_lock;

};


