#pragma once

#include <Core/TObject.h>
#include <Container/TString.h>
#include <Container/TArray.h>
#include <Container/THashMap.h>

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
class TSkinModelAction;
class THanger;
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

	TSkinModel * Clone();

	void Init();
	void Release();

	bool Load(const char * pFile, int skinFlag);
	bool Load(TFile * pFile, int skinFlag);

	TSkeleton* LoadSkeleton(const char * skeletonFile);
	TSkin * LoadSkin(const char* skinFile, bool autoFree);
	THanger * LoadHanger(TFile * pFile);

	bool Save(TFile * pFile);
	bool Save(const char * pFile);

	void Render();

	int GetSkinModelID() { return m_modelID; }
private:
	int	m_modelID;			//Model ID
	int m_version;			//File version
	int m_numSkin;			//skin num
	int m_numAction;		//action num
	int m_numHanger;		//hanger num
	int m_numProp;			//prop num
	int m_refcount;			//reference count
	TString m_fileName;		//Model file name
	TString m_phyFileName;	//Physique file name
	TString m_tckDirName;	//Track Data Directory name

	TSkeleton* m_pSkeleton;
	TArray<TSkin*> m_skins;
	TArray<TSkinModelAction*> m_actions;
	THashMap<TString, TString> m_props;
};





