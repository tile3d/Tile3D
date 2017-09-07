#include "TSkeletonHook.h"
#include <File/TFile.h>
#include <Util/TLog.h>

//	Load model from file
bool TSkeletonHook::Load(TFile* pFile)
{
	//	Read hook name
	pFile->ReadString(m_hookName);

	//	Read hook data
	TSkeletonHookData hookData;
	int count;

	if (!pFile->Read(&hookData, sizeof(hookData), &count) || count != sizeof(hookData))
	{
		TLog::Log(LOG_ERR, "SkinModel", "TSkeletonHook::Load, failed to load the hook data filename=%s", pFile->GetRelativeFileName());
		return false;
	}

	m_hookType = hookData.m_hookType;
	m_boneIndex = hookData.m_boneIndex;
	m_hookMat = hookData.m_hookMat;
	return true;
}

//	Load model from file
bool TSkeletonHook::Save(TFile* pFile)
{
	//	Read hook name
	pFile->WriteString(m_hookName);

	//	Read hook data
	TSkeletonHookData hookData;
	hookData.m_hookType = m_hookType;
	hookData.m_boneIndex = m_boneIndex;
	hookData.m_hookMat = m_hookMat;

	int count;
	if (!pFile->Write(&hookData, sizeof(hookData), &count) || count != sizeof(hookData))
	{
		TLog::Log(LOG_ERR, "SkinModel", "TSkeletonHook::Save, failed to write the hook data filename=%s", pFile->GetRelativeFileName());
		return false;
	}
	return true;
}

