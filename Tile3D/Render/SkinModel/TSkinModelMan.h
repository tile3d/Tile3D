#pragma once

#include <Core/TObject.h>
#include <Core/Lock/TMutexLock.h>


class TFile;
class TSkinModel;
class TSkinModelMan : public TObject
{
public:
	static TSkinModelMan * GetInstance() {
		static TSkinModelMan skinModelMan;
		return &skinModelMan;
	}

	TSkinModel * LoadSkinModel(const char * pFile);
	TSkinModel * LoadSkinModel(TFile * pFile);


private:



};