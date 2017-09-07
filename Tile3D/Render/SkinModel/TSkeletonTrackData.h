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

template<typename T> class TSkeletonTrackData
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
		m_keyFrames = nullptr;
		m_trackLength = 0;
		m_trackDataCompAlgorithm = TRACKDATA_COMPRESS_REMOVE_RAWDATA;
	}

	~TSkeletonTrackData() {
		if (m_keyFrames) {
			delete[] m_keyFrames;
			m_keyFrames = nullptr;
		}

		if (m_segments) {
			delete[] m_segments;
			m_segments = nullptr;
		}

		if (m_keyFrameIDs) {
			delete[] m_keyFrameIDs;
			m_keyFrameIDs = nullptr;
		}
	}

	virtual bool LoadBoneTrackData(TFile * pFile, int bone_id, int version);

public:
	int m_frameRate;
	int m_keyNum;
	int m_segmentNum;
	int m_trackLength;
	int m_trackEndKeyID;
	int m_trackDataCompAlgorithm;

	T * m_keyFrames;
	TSkeletonTrackSegement * m_segments;
	unsigned short * m_keyFrameIDs;


};


class TSkeletonTrackDataPos : public TSkeletonTrackData<TVector3>
{
public:
	TSkeletonTrackDataPos() {
	}

	~TSkeletonTrackDataPos() {
	}

};

class TSkeletonTrackDataRot : public TSkeletonTrackData<TQuaternion>
{
public:
	TSkeletonTrackDataRot() {
	}

	~TSkeletonTrackDataRot() {
	}
};

