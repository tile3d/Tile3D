#pragma once

#include <Container/THashMap.h>
#include <Container/TString.h>
#include <Core/Lock/TMutexLock.h>


//TBD
//1) supoort to reload conf file
//2) support to put new key/value pair to the conf (need lock)
//3) append a conf file to a existing file
//
class TConf;
class TConfMan
{
public:
	static TConf* GetInstance(TString & filename);

private:
	static TMutexLock m_lock;
	static THashMap<TString, TConf*> m_confs;
};


class TConf
{
public:
	TConf() {}
	~TConf() {}

	bool Load(const TString & filename);
	TString Find(const TString& section_name, const TString& key);


private:
	typedef THashMap<TString, TString> VALUE_MAP;
	typedef THashMap<TString, VALUE_MAP> SECTION_MAP;
	SECTION_MAP m_sections;


};

