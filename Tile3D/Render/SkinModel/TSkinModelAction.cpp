#include "TSkinModelAction.h"
#include <Util/TLog.h>
#include <File/TFile.h>
#include <File/TFileDir.h>

TSkinModelAction::TSkinModelAction()
{
	m_startFrame = 0;
	m_endFrame = 0;
	m_frameRate = 0;
	m_startTime = 0;
	m_endTime = 0;
	m_actionID = 0;
	m_trackID = 0;
	m_actionTime = 0;
}

TSkinModelAction::~TSkinModelAction()
{

}

bool TSkinModelAction::Load(TFile * pFile, int version, const char * tckDirName)
{
	pFile->ReadString(m_actionName);
	m_actionID = TFileDir::GetInstance()->GetIDFromFileName(m_actionName);

	if (version < 6) {
		TSkinModelActionData ad;
		int count;

		if (!pFile->Read(&ad, sizeof(ad), &count) || count != sizeof(ad)) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkinModelAction::Load,  Failed to load the action data, filename=%s  actionName=%s version=%d",
				pFile->GetRelativeFileName(), m_actionName, version);
			return false;
		}

		if (ad.m_jointNum >= 1) {
			TSkinModelActionJoint aj;
			if (!pFile->Read(&aj, sizeof(aj), &count) || count != sizeof(aj)) {
				TLog::Log(LOG_ERR, "SkinModel", "TSkinModelAction::Load,  Failed to load the action data joint, filename=%s  actionName=%s",
					pFile->GetRelativeFileName(), m_actionName);
				return false;
			}

			SetStartAndEndFrame(aj.m_startFrame, aj.m_endFrame);
			if (ad.m_jointNum > 1) {
				pFile->Seek((ad.m_jointNum - 1) * sizeof(TSkinModelActionData), TFile::TFILE_SEEK_CUR);
			}
		}

	}
	else if (version < 9) {
		TSkinModelActionData6 ad;
		int count;

		if (!pFile->Read(&ad, sizeof(ad), &count) || count != sizeof(ad)) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkinModelAction::Load,  Failed to load the action data, filename=%s  actionName=%s version=%d",
				pFile->GetRelativeFileName(), m_actionName, version);
			return false;
		}
		SetStartAndEndFrame(ad.m_startFrame, ad.m_endFrame);
	}
	else {
		TSkinModelActionData9 ad;
		int count;

		if (!pFile->Read(&ad, sizeof(ad), &count) || count != sizeof(ad)) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkinModelAction::Load,  Failed to load the action data, filename=%s  actionName=%s version=%d", 
				pFile->GetRelativeFileName(), m_actionName, version);
			return false;
		}

		m_frameRate = ad.m_frameRate;
		SetStartAndEndFrame(ad.m_startFrame, ad.m_endFrame);
	}

	if (version >= 7) {
		TString str;
		pFile->ReadString(str);
		if (tckDirName[0]) {
			str = "\\" + str;
			str = tckDirName + str;
		}
		SetTrackSetFileName(str);
	}
	return true;
}

void TSkinModelAction::SetStartAndEndFrame(float startFrame, float endFrame)
{
	m_startFrame = startFrame;
	m_endFrame = endFrame;
	m_startTime = GetFrameTime(m_startFrame);
	m_endTime = GetFrameTime(m_endFrame);
	m_actionTime = m_endTime - m_startTime + 1;
}


void TSkinModelAction::SetTrackSetFileName(TString& fileName) {
	m_trackName = fileName;
	m_trackID = TFileDir::GetInstance()->GetIDFromFileName(m_trackName);
}
