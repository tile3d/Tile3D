#pragma once

#include <Container/THashMap.h>
#include <Core/Lock/TMutexLock.h>

class TSkinMesh;
class TFile;
class TSkinMeshMan
{
public:
	static TSkinMeshMan * GetInstance() {
		static TSkinMeshMan skinmeshman;
		return &skinmeshman;
	}

	TSkinMesh* LoadSkinMesh(const char * skinFile);
	TSkinMesh* LoadSkinMesh(TFile * pFile);

private:
	TSkinMesh* FindSkinByFile(const char * skinFile);
	TSkinMesh* FindSkinByID(int skeletonFileID);
	TSkinMesh * CreateSkin(TFile * pFile);

private:
	THashMap<int, TSkinMesh*> m_skinMeshs;
	TMutexLock m_lock;

};


