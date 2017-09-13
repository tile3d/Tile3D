#pragma once

#include <Core/TObject.h>
#include <Container/TString.h>


struct TSkinModelActionData
{
	int m_actGroup;		//action group id
	int m_jointNum;		//joint nums
	int m_flags;	
	TSkinModelActionData() {
		m_actGroup = 0;
		m_jointNum = 0;
		m_flags = 0;
	}
};

struct TSkinModelActionJoint
{
	int m_jointIndex;
	float m_startFrame;
	float m_endFrame;
	int m_loopNum;		//-1 means loop forever;
	float m_speedFactor;
};


struct TSkinModelActionData6
{
	float m_startFrame;
	float m_endFrame;

	TSkinModelActionData6() {
		m_startFrame = 0;
		m_endFrame = 0;
	}
};

struct TSkinModelActionData9
{
	float m_startFrame;
	float m_endFrame;
	int m_frameRate;

	TSkinModelActionData9() {
		m_startFrame = 0;
		m_endFrame = 0;
		m_frameRate = 0;
	}
};

class TFile;
class TSkinModelAction : public TObject
{
public:
	TSkinModelAction();
	~TSkinModelAction();

	bool Load(TFile * pFile, int verison, const char * tckDirName);


	void SetStartAndEndFrame(float startFrame, float endFrame);
	void SetTrackSetFileName(TString& fileName);


	int GetFrameTime(float frame) {
		if (m_frameRate <= 0) return 0;
		return (int)(frame*1000.0f / m_frameRate);
	}


private:
	float m_startFrame;
	float m_endFrame;
	int m_frameRate;
	int m_startTime;
	int m_endTime;
	int m_actionTime;
	int m_actionID;
	int m_trackID;
	TString m_actionName;
	TString m_trackName;
};
