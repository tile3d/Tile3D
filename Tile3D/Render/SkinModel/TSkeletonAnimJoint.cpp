#include "TSkeletonAnimJoint.h"

TSkeletonAnimJoint::TSkeletonAnimJoint()
{

}

TSkeletonAnimJoint::~TSkeletonAnimJoint()
{

}


bool TSkeletonAnimJoint::Load(TFile* pFile)
{
	return TSkeletonJoint::Load(pFile);
}


bool TSkeletonAnimJoint::Save(TFile* pFile)
{
	return TSkeletonJoint::Save(pFile);
}

