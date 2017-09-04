#include "TJoint.h"
#include "TAnimJoint.h"
#include "TControllerJoint.h"
#include "TSprintJoint.h"
#include "TAxisSprintJoint.h"
#include <Util/TAssert.h>
#include <File/TFile.h>
#include <File/TFileDir.h>
#include <Util/TLog.h>


TJoint::TJoint()
{
	m_jointID = 0;
	m_jointType = 0;
	m_parentBone = 0;
	m_childBone = 0;
	m_siblingJoint = 0;
	m_pSkeleton = nullptr;
}


TJoint::~TJoint()
{
	m_pSkeleton = nullptr;
}

TJoint* TJoint::CreateJointByClassID(int classID)
{
	TJoint * pJoint = nullptr;

	switch (classID) 
	{
	case CLASSID_ANIMJOINT:
		pJoint = new TAnimJoint();
	break;

	case CLASSID_CONTROLLEDJOINT:
		pJoint = new TControllerJoint();
	break;
	
	case CLASSID_SPRINGJOINT:
		pJoint = new TSprintJoint();
	break;
	
	case CLASSID_AXISSPRINGJOINT:
		pJoint = new TAxisSprintJoint();
	break;

	default:
		TAssert(0);
	break;
	}
	return pJoint;
}

TJoint* TJoint::CreateJointByType(int type)
{
	TJoint * pJoint = nullptr;

	switch (type)
	{
	case TJoint::TJOINT_TYPE_ANIM:
		pJoint = new TAnimJoint();
		break;

	case TJoint::TJOINT_TYPE_CONTROL:
		pJoint = new TControllerJoint();
		break;

	case TJoint::TJOINT_TYPE_SPRING:
		pJoint = new TSprintJoint();
		break;

	case TJoint::TJOINT_TYPE_AXISSPRING:
		pJoint = new TAxisSprintJoint();
		break;

	default:
		TAssert(0);
		break;
	}
	return pJoint;
}

bool TJoint::Load(TFile * pFile)
{
	if (!pFile->ReadString(m_jointName)) {
		TLog::Log(LOG_ERR, "SkinModel", "TJoint::Load,  Failed to read the joint name, filename=%s", pFile->GetRelativeFileName());
		return false;
	}

	int count;
	TJointData data;
	if (!pFile->Read(&data, sizeof(data), &count) || count != sizeof(data)) {
		TLog::Log(LOG_ERR, "SkinModel", "TJoint::Load,  Failed to read the joint data, filename=%s", pFile->GetRelativeFileName());
		return false;
	}


	m_jointID = TFileDir::GetInstance()->GetIDFromFileName(m_jointName);
	m_parentBone = data.m_parentBone;
	m_childBone = data.m_childBone;
	m_siblingJoint = data.m_siblingJoint;
	return true;
}

bool TJoint::Save(TFile * pFile)
{
	if (!pFile->WriteString(m_jointName)) {
		TLog::Log(LOG_ERR, "SkinModel", "TJoint::Load,  Failed to write the joint name, filename=%s", pFile->GetRelativeFileName());
		return false;
	}

	int count;
	TJointData data;
	data.m_parentBone = m_parentBone;
	data.m_childBone = m_childBone;
	data.m_siblingJoint = m_siblingJoint;
	if (!pFile->Write(&data, sizeof(data), &count) || count != sizeof(data)) {
		TLog::Log(LOG_ERR, "SkinModel", "TJoint::Load,  Failed to write the joint data, filename=%s", pFile->GetRelativeFileName());
		return false;
	}
	return true;
}


