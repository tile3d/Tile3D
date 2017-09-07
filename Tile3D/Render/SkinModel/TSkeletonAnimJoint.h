#pragma once

#include "TSkeletonJoint.h"

class TFile;
class TSkeletonAnimJoint : public TSkeletonJoint
{
public:
	TSkeletonAnimJoint();
	~TSkeletonAnimJoint();

	virtual bool Load(TFile* pFile);
	virtual bool Save(TFile* pFile);

private:

};
