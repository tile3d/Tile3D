#pragma once

#include <Core/TObject.h>
#include <Container/TString.h>

#pragma pack(push, 1)
struct TSkinModelHeader
{
	int	m_flags;
	int m_version;
	int m_numSkin;
	int m_numAction;
	int m_numHanger;
	int m_numProp;
	char m_reserved[60];
};

struct TSkinModelPhyfileHeader
{
	int m_flags;
	int m_version;
	int m_numHitBox;
	int m_numPhyShape;
	char m_reserved[64];
};

#pragma pack(pop)


//TBD
//1) skinmodel release logic
//
//
//
class TFile;
class TSkeleton;
class TSkin;
class TSkinModel : public TObject
{
public:
	enum
	{
		SKINMODEL_IDENTIFY = (('A' << 24) | ('S' << 16) | ('M' << 8) | 'D'),
		SKINMODEL_VERSION = 9,
		SKINPHY_IDENTIFY = (('A' << 24) | ('S' << 16) | ('M' << 8) | 'Y'),
		SKINPHY_VERSION = 1
	};

	enum SKIN_LOAD_FLAG
	{
		SKIN_LOAD_DEFAULT,	//Load Skin By Skin Manager
		SKIN_LOAD_NOSKIN,	//Don't load skin
		SKIN_LOAD_UNIQUESKIN,	//Load a unique skin
	};

	TSkinModel();
	virtual ~TSkinModel();

	bool Init();
	void Release();

	bool Load(const char * pFile, int skinFlag);
	bool Load(TFile * pFile, int skinFlag);

	TSkeleton* LoadSkeleton(const char * skeletonFile);
	TSkin * LoadSkin(const char* skinFile, bool autoFree);

	bool Save(TFile * pFile);
	bool Save(const char * pFile);

private:

	TString m_fileName;		//Model file name
	TString m_phyFileName;	//Physique file name
	int	m_modelID;			//Model ID
	int m_version;			//File version

	TSkeleton* m_pSkeleton;
};



