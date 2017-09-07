#pragma once

#include <Core/TObject.h>
#include <Container/TString.h>
#include <Container/THashMap.h>
#include "TSkeletonTrackData.h"

struct TSkeletonTrackHeader
{
	int m_boneTrackNum;
	int m_animStart;
	int m_animEnd;
	int m_animFPS;
};


struct TSkeletonTrackBone
{
	TSkeletonTrackDataPos* m_pPosTrack;
	TSkeletonTrackDataRot* m_pRotTrack;
};

class TFile;
class TSkeletonTrack : public TObject
{
public:
	TSkeletonTrack();
	~TSkeletonTrack();

	bool Load(const char* fileName);
	bool Load(TFile * pFile);
	bool LoadBoneTrack(TFile * pFile, int id);

	bool Save(const char* fileName);
	bool Save(TFile * pFile);

	void SetFileNameAndID(const char* fileName, int fileID) {
		m_fileName = fileName;
		m_fileNameID = fileID;
	}

	void SetBoneFile(bool oldBoneFile) { m_oldBonefile = oldBoneFile; }
	const char * GetFileName() { return m_fileName; }

	void SetAnimFrameRange(int start, int end) {
		m_animStart = start;
		m_animEnd = end;
	}

	void SetAnimFPS(int fps) {
		m_animFPS = fps;
	}

private:
	int m_version;
	int m_animStart;
	int m_animEnd;
	int m_animFPS;
	int m_fileNameID;
	bool m_oldBonefile;
	TString m_fileName;
	THashMap<int, TSkeletonTrackBone*> m_boneTracks;
};


