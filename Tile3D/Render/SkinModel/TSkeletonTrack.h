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


private:
	int m_version;
	int m_animStart;
	int m_animEnd;
	int m_animFPS;
	int m_fileNameID;
	TString m_fileName;
	THashMap<int, TSkeletonTrackBone> m_boneTracks;
};


