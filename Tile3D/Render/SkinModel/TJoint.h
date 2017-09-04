#pragma once

#include <Core/TObject.h>
#include <Container/TString.h>


struct TJointData
{
	int m_parentBone;
	int m_childBone;
	int m_siblingJoint;
};

class TSkeleton;
class TFile;
class TJoint : public TObject
{
public:
	enum TJOINT_TYPE
	{
		TJOINT_TYPE_ANIM,
		TJOINT_TYPE_CONTROL,
		TJOINT_TYPE_SPRING,
		TJOINT_TYPE_AXISSPRING,
	};

public:
	TJoint();
	~TJoint();

	void Init(TSkeleton * pSkeleton) {
		m_pSkeleton = pSkeleton;
	}

	virtual bool Load(TFile* pFile);
	virtual bool Save(TFile* pFile);

	static TJoint* CreateJointByClassID(int classID);
	static TJoint* CreateJointByType(int type);

private:
	TSkeleton * m_pSkeleton;
	TString m_jointName;
	int m_jointID;
	int m_jointType;
	int m_parentBone;
	int m_childBone;
	int m_siblingJoint;
};

