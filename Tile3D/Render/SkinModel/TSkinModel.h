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


	TSkinModel();
	virtual ~TSkinModel();

	bool Init();
	void Release();

	bool Load(const char * pFile, int skinFlag);
	bool Load(TFile * pFile, int skinFlag);

	TSkeleton* BindSkeleton(const char * skeletonFile);

	bool Save(TFile * pFile);
	bool Save(const char * pFile);

private:

	TString m_fileName;		//Model file name
	TString m_phyFileName;	//Physique file name
	int	m_modelID;			//Model ID
	int m_version;			//File version

	TSkeleton* m_pSkeleton;
};



