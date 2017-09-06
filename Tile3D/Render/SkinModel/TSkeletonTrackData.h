#pragma once
#include <Math/TVector3.h>
#include <Math/TQuaternion.h>

class TFile;

struct TSkeletonTrackDataHeader
{
	int m_keyNum;
	int m_segmentNum;
	int m_frameRate;
};

struct TSkeletonTrackSegement
{
	int m_startTime;
	int m_endTime;
	int m_startKey;
	int m_endKey;
};

class TSkeletonTrackData
{
public:
	enum TRACKDATA_COMPRESS_ALGO
	{
		TRACKDATA_COMPRESS_REMOVE_RAWDATA,
		TRACKDATA_COMPRESS_REMOVE_IDENTIFICAL_KEYS, 
	};
public:
	TSkeletonTrackData() {
		m_frameRate = 0;
		m_keyNum = 0;
		m_segmentNum = 0;
		m_segments = nullptr;
		m_keyFrameIDs = nullptr;
		m_trackLength = 0;
		m_trackDataCompAlgorithm = TRACKDATA_COMPRESS_REMOVE_RAWDATA;
	}

	~TSkeletonTrackData() {
		if (m_segments) {
			delete[] m_segments;
			m_segments = nullptr;
		}

		if (m_keyFrameIDs) {
			delete[] m_keyFrameIDs;
			m_keyFrameIDs = nullptr;
		}
	}

	virtual bool LoadBoneTrackPos(TFile * pFile, int bone_id, int version) { return true; }

public:
	int m_frameRate;
	int m_keyNum;
	int m_segmentNum;
	int m_trackLength;
	int m_trackEndKeyID;
	int m_trackDataCompAlgorithm;

	TSkeletonTrackSegement * m_segments;
	unsigned short * m_keyFrameIDs;


};


class TSkeletonTrackDataPos : public TSkeletonTrackData
{
public:
	TSkeletonTrackDataPos() {
		m_keyFrames = nullptr;
	}

	~TSkeletonTrackDataPos() {
		if (m_keyFrames) {
			delete[] m_keyFrames;
			m_keyFrames = nullptr;
		}
	}

	virtual bool LoadBoneTrackPos(TFile * pFile, int bone_id, int version);

public:
	TVector3* m_keyFrames;
};

class TSkeletonTrackDataRot : public TSkeletonTrackData
{
public:
	TSkeletonTrackDataRot() {
		m_keyFrames = nullptr;
	}

	~TSkeletonTrackDataRot() {
		if (m_keyFrames) {
			delete[] m_keyFrames;
			m_keyFrames = nullptr;
		}
	}

	virtual bool LoadBoneTrackPos(TFile * pFile, int bone_id, int version);

public:
	TQuaternion* m_keyFrames;
};

