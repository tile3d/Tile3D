#include "TSkeletonJoint.h"
#include "TSkeletonAnimJoint.h"
#include "TSkeletonControllerJoint.h"
#include "TSkeletonSprintJoint.h"
#include "TSkeletonAxisSprintJoint.h"
#include <Util/TAssert.h>
#include <File/TFile.h>
#include <File/TFileDir.h>
#include <Util/TLog.h>


TSkeletonJoint::TSkeletonJoint()
{
	m_jointID = 0;
	m_jointType = 0;
	m_parentBone = 0;
	m_childBone = 0;
	m_siblingJoint = 0;
	m_pSkeleton = nullptr;
}


TSkeletonJoint::~TSkeletonJoint()
{
	m_pSkeleton = nullptr;
}

TSkeletonJoint* TSkeletonJoint::CreateJointByClassID(int classID)
{
	TSkeletonJoint * pJoint = nullptr;

	switch (classID) 
	{
	case CLASSID_ANIMJOINT:
		pJoint = new TSkeletonAnimJoint();
	break;

	case CLASSID_CONTROLLEDJOINT:
		pJoint = new TSkeletonControllerJoint();
	break;
	
	case CLASSID_SPRINGJOINT:
		pJoint = new TSkeletonSprintJoint();
	break;
	
	case CLASSID_AXISSPRINGJOINT:
		pJoint = new TSkeletonAxisSprintJoint();
	break;

	default:
		TAssert(0);
	break;
	}
	return pJoint;
}

TSkeletonJoint* TSkeletonJoint::CreateJointByType(int type)
{
	TSkeletonJoint * pJoint = nullptr;

	switch (type)
	{
	case TSkeletonJoint::TJOINT_TYPE_ANIM:
		pJoint = new TSkeletonAnimJoint();
		break;

	case TSkeletonJoint::TJOINT_TYPE_CONTROL:
		pJoint = new TSkeletonControllerJoint();
		break;

	case TSkeletonJoint::TJOINT_TYPE_SPRING:
		pJoint = new TSkeletonSprintJoint();
		break;

	case TSkeletonJoint::TJOINT_TYPE_AXISSPRING:
		pJoint = new TSkeletonAxisSprintJoint();
		break;

	default:
		TAssert(0);
		break;
	}
	return pJoint;
}

bool TSkeletonJoint::Load(TFile * pFile)
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

bool TSkeletonJoint::Save(TFile * pFile)
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


