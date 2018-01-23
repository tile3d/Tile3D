#include "TConf.h"
#include <Core/Lock/TScopeLock.h>
#include <Sys/TSysFile.h>
#include <stdio.h>

TConf* TConfMan::GetInstance(TString & conf_name)
{
	TScopeLock lock(m_lock);
	TConf** pConf = m_confs.Find(conf_name);
	if (pConf != nullptr) {
		return *pConf;
	}
	else {
		TConf * pNewConf = new TConf();
		if (pNewConf->Load(conf_name)) {
			m_confs.Put(conf_name, pNewConf);
		}
		else {

		}
	}
	return nullptr;
}

bool TConf::Load(const TString & filename)
{
	FILE * fp = fopen(filename.ToString(), "r");
	if (fp == nullptr) return false;

	TString line;
	TString section;

	//TBD
//	char* line = nullptr;

	return true;

}

TString TConf::Find(const TString& section_name, const TString& key)
{
	VALUE_MAP * pValues = m_sections.Find(section_name);
	if (pValues == nullptr) {
		return TString();
	}

	TString * pString = pValues->Find(key);
	if (pString == nullptr) {
		return TString();
	}
	return *pString;
}

