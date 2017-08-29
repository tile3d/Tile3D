#include "TSkinModel.h"
#include <Util/TLog.h>
#include <File/TFile.h>
#include <File/TFileImage.h>
#include <File/TFileDir.h>

TSkinModel::TSkinModel()
{

}


TSkinModel::~TSkinModel()
{

}


bool TSkinModel::Load(const char * pFile, int skinFlag)
{
	TFileImage file;
	if (!file.Open("", pFile, TFile::TFILE_OPENEXIST | TFile::TFILE_BINARY | TFile::TFILE_TEMPMEMORY)) {
		file.Close();
		TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  Can't open file [%s].", pFile);
		return false;
	}

	if (!Load(&file, skinFlag)) {
		file.Close();
		TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  Failed to load the file [%s].", pFile);
		return false;
	}

	file.Close();
	return true;
}


bool TSkinModel::Load(TFile * pFile, int skinFlag)
{
	TSkinModelHeader header;
	int readCnt = 0;

	if (!(pFile->Read(&header, sizeof(header), &readCnt)) || readCnt != sizeof(header)) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  Failed to read the file header [%s].", pFile->GetFileName());
		return false;
	}

	if (header.m_flags != SKINMODEL_IDENTIFY || header.m_version != SKINMODEL_VERSION) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  Invalid file identify or version [%s].", pFile->GetFileName());
		return false;
	}

	m_fileName = pFile->GetRelativeFileName();
	m_modelID = TFileDir::GetInstance()->GetIDFromFileName(m_fileName);
	m_version = header.m_version;

	TString bonFile, filePath;
	TFileDir::GetInstance()->GetFilePath(pFile->GetRelativeFileName(), filePath);
	pFile->ReadString(bonFile);
	if (bonFile.GetLength() > 0) {
		if (filePath.GetLength() > 0) {
			bonFile = filePath + "\\" + bonFile;
		}
	}

	if (bonFile.GetLength() <= 0) {
		TLog::Log(LOG_ERR, "SkinModel", "TSkinModel::Load,  Invalid bon file [%s].", pFile->GetFileName());
		return false;
	}

	if (LoadSkeleton(bonFile)) {
		return false;
	}




}