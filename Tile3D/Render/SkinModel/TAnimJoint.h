#pragma once

#include "TJoint.h"

class TFile;
class TAnimJoint : public TJoint
{
public:
	TAnimJoint();
	~TAnimJoint();

	virtual bool Load(TFile* pFile);
	virtual bool Save(TFile* pFile);

private:

};