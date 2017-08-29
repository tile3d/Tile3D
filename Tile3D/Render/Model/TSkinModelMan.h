#pragma once

#include <Core/TObject.h>

class TSkeleton;
class TFile;
class TSkinMeshMan : public TObject
{
public:
	TSkinMeshMan * GetInstance() {
		static TSkinMeshMan skinMeshMan;
		return &skinMeshMan;
	}

	TSkeleton * FindSkeleton(const char * pFile);
	TSkeleton * FindSkeleton(TFile * pFile);


private:

};