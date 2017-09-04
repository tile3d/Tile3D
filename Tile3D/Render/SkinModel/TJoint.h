#pragma once

#include <Core/TObject.h>


class TSkeleton;
class TFile;
class TJoint : public TObject
{
public:
	TJoint();
	~TJoint();

	void Init(TSkeleton * pSkeleton) {
		m_pSkeleton = pSkeleton;
	}

	bool Load(TFile * pFile);

	static TJoint* CreateJointByClassID(int classID);
	static TJoint* CreateJointByType(int type);

private:
	TSkeleton * m_pSkeleton;
};

