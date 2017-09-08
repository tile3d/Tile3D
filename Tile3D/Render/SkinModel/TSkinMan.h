#pragma once

#include <Container/THashMap.h>
#include <Core/Lock/TMutexLock.h>

class TSkin;
class TFile;
class TSkinMan
{
public:
	static TSkinMan * GetInstance() {
		static TSkinMan skinman;
		return &skinman;
	}

	TSkin* LoadSkin(const char * skinFile);
	TSkin* LoadSkin(TFile * pFile);

private:
	TSkin* FindSkinByFile(const char * skinFile);
	TSkin* FindSkinByID(int skeletonFileID);
	TSkin * CreateSkin(TFile * pFile);

private:
	THashMap<int, TSkin*> m_skins;
	TMutexLock m_lock;
	
};


