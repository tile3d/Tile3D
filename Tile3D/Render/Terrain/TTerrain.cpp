#include "TTerrain.h"
#include "TTerrainFile.h"
#include <Common/TLog.h>
#include <File/TFile.h>
#include <File/TFileImage.h>


bool TTerrain::Init(const char* fileName, bool enableLightMap, const TVector3& vLoadCenter, float loadDistance,
	float viewDistance, float lodDistance1, float lodDistance2, bool multiThreadLoad, bool enableDynamicStream)
{
	m_enableLightMap = enableLightMap;

	TFileImage trnFile;
	if (!trnFile.Open("", fileName, TFile::TFILE_OPENEXIST | TFile::TFILE_BINARY | TFile::TFILE_TEMPMEMORY))
	{
		TLog::Log(LOG_INFO, "TTerrain::Init", "Failed to open file %s", fileName);
		return false;
	}

	int count;
	//	¶ÁÈ¡IDºÍ°æ±¾
	TRN2FILEIDVER idVer;
	if (!trnFile.Read(&idVer, sizeof(idVer), &count))
	{
		TLog::Log(LOG_INFO, "TTerrain::Init", "Failed to read file version");
		trnFile.Close();
		return false;
	}

	if (idVer.m_identify != TRN2FILE_IDENTIFY)
	{
		TLog::Log(LOG_INFO, "TTerrain::Init", "File format error");
		trnFile.Close();
		return false;
	}

	if (idVer.m_version > TRN2FILE_VERSION)
	{
		TLog::Log(LOG_INFO, "TTerrain::Init", "Version %d expected but %d is given", TRN2FILE_VERSION, idVer.m_version);
		trnFile.Close();
		return false;
	}


	return true;

}