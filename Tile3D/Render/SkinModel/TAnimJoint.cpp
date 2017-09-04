#include "TAnimJoint.h"

TAnimJoint::TAnimJoint()
{

}

TAnimJoint::~TAnimJoint()
{

}


bool TAnimJoint::Load(TFile* pFile)
{
	return TJoint::Load(pFile);
}


bool TAnimJoint::Save(TFile* pFile)
{
	return TJoint::Save(pFile);
}

