#pragma once

#include <Core/TObject.h>
#include <Container/TString.h>
#include <Math/TMatrix4.h>

struct TSkeletonHookData
{
	int m_hookType;
	int m_boneIndex;
	TMatrix4 m_hookMat;
};

class TFile;
class TSkeleton;
class TSkeletonHook
{
public:
	TSkeletonHook(TSkeleton * pSkeleton) {
		m_pSkeleton = pSkeleton;
		m_hookType = 0;
		m_boneIndex = 0;
	}

	~TSkeletonHook() {

	}

	bool Load(TFile* pFile);
	bool Save(TFile* pFile);

private:
	TString m_hookName;
	TSkeleton * m_pSkeleton;
	int m_hookType;
	int m_boneIndex;
	TMatrix4 m_hookMat;
};

