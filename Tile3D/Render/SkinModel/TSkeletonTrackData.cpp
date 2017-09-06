#include "TSkeletonTrackData.h"
#include <File/TFileImage.h>
#include <Util/TLog.h>

bool TSkeletonTrackDataPos::LoadBoneTrackPos(TFile * pFile, int bone_id, int version)
{
	TSkeletonTrackDataHeader data;
	int count;
	if (!pFile->Read(&data, sizeof(data), &count) || count != sizeof(data)) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkeletonTrackDataPos::LoadBoneTrackPos,  fail to load the bone track pos header, bone_id=%d", bone_id);
		return false;
	}
	m_frameRate = data.m_frameRate;
	m_keyNum = data.m_keyNum;
	m_keyFrames = new TVector3[data.m_keyNum];
	m_segmentNum = data.m_segmentNum;
	if (data.m_segmentNum > 0) {
		m_segments = new TSkeletonTrackSegement[data.m_segmentNum];
	}

	if (!pFile->Read(m_keyFrames, m_keyNum*sizeof(TVector3), &count) || count != m_keyNum*sizeof(TVector3)) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkeletonTrackDataPos::LoadBoneTrackPos,  fail to load the bone track pos key frames, bone_id=%d", bone_id);
		return false;
	}

	if (version < 2) {
		if (!pFile->Read(m_segments, m_segmentNum*sizeof(TSkeletonTrackSegement), &count)
			|| count != m_segmentNum*sizeof(TSkeletonTrackSegement)) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkeletonTrackDataPos::LoadBoneTrackPos,  fail to load the bone track pos segments, bone_id=%d", bone_id);
			return false;
		}

		int startFrame = (int)((m_segments[0].m_startTime + 1)*m_frameRate*0.001f);
		if (m_keyFrameIDs) {
			delete[] m_keyFrameIDs;
		}
		m_keyFrameIDs = new unsigned short[data.m_keyNum];

		m_keyFrameIDs[0] = 0;
		for (int i = 1; i < m_keyNum; i++) {
			m_keyFrameIDs[i] = startFrame + i;
		}

		m_trackLength = m_segments[m_segmentNum - 1].m_endTime;
		m_trackEndKeyID = (int)((m_trackLength + 1)*m_frameRate*0.001f);
		m_trackDataCompAlgorithm = TRACKDATA_COMPRESS_REMOVE_IDENTIFICAL_KEYS;
	}
	else {
		if (!pFile->Read(&m_trackLength, sizeof(int), &count) || count != sizeof(int)) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkeletonTrackDataPos::LoadBoneTrackPos,  fail to load the bone track length ");
			return false;
		}

		if (!pFile->Read(&m_trackDataCompAlgorithm, sizeof(int), &count) || count != sizeof(int)) {
			TLog::Log(LOG_ERR, "SkinModel", "TSkeletonTrackDataPos::LoadBoneTrackPos,  fail to load the bone track comp algo");
			return false;
		}

		if (m_trackDataCompAlgorithm != TRACKDATA_COMPRESS_REMOVE_RAWDATA) {
			if (m_keyFrameIDs) {
				delete[] m_keyFrameIDs;
			}
			m_keyFrameIDs = new unsigned short[m_keyNum];
			if (!pFile->Read(m_keyFrameIDs, m_keyNum*sizeof(unsigned short), &count) || count != data.m_keyNum*sizeof(unsigned short)) {
				TLog::Log(LOG_ERR, "SkinModel", "TSkeletonTrackDataPos::LoadBoneTrackPos,  fail to load the bone track frame IDs");
				return false;
			}

			if (m_trackLength == 0) {
				m_trackLength = (int)((m_keyNum - 1)*1000.0f / m_frameRate);
			}
			m_trackEndKeyID = (int)((m_trackLength + 1)*m_frameRate*0.001f);

			delete[] m_segments;
			m_segments = nullptr;
			m_segmentNum = 0;
		}
	}
	return true;
}


