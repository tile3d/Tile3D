#pragma once

#include <Core/TObject.h>
#include <Core/Lock/TMutexLock.h>
#include <Container/THashMap.h>

class TFile;
class TSkinModel;
class TSkinModelMan : public TObject
{
public:
	static TSkinModelMan * GetInstance() {
		static TSkinModelMan skinModelMan;
		return &skinModelMan;
	}

	TSkinModel * LoadSkinModel(const char * pFile, int skinFlag);
	TSkinModel * LoadSkinModel(TFile * pFile, int skinFlag);

private:
	TSkinModel* FindSkinModelByFile(const char * skinFile);
	TSkinModel* FindSkinModelByID(int skinModelID);
	TSkinModel * CreateSkinModel(TFile * pFile, int skinFlag);

private:
	THashMap<int, TSkinModel*> m_skinModels;
	TMutexLock m_lock;
};
