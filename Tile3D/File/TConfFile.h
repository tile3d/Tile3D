#pragma once

#include <Container\TString.h>
#include <Container\TArray.h>

class TFile;
class TConfFile
{
public:		//	Types

	struct TConfPair
	{
		TString		m_key;		//	Key name
		TString		m_value;	//	Value string
	};

	struct TConfSection
	{
		bool				m_comment;	//	true, Comment section
		TString				m_name;	//	Section name
		TArray<TConfPair*>	m_keys;		//	Keys
	};

public:		//	Constructions and Destructions

	TConfFile();
	virtual ~TConfFile();

public:		//	Operaitons
	//	Open ini file
	bool Open(const char* szFile);
	bool Open(TFile* pFile);
	
	//	Close file
	void Close();

	//	Save modified data
	bool Save(const char* szFile);
	bool Save(TFile* pFile);

	//	Get values
	int GetValueAsInt(const char* szSect, const char* szKey, int iDefault);
	TString GetValueAsString(const char* szSect, const char* szKey, const char* szDefault = NULL);
	float GetValueAsFloat(const char* szSect, const char* szKey, float fDefault);

	//	Write values
	bool WriteIntValue(const char* szSect, const char* szKey, int iValue);
	bool WriteStringValue(const char* szSect, const char* szKey, const char* szValue);
	bool WriteFloatValue(const char* szSect, const char* szKey, float fValue);

	//	Some facility functions
	bool GetValueAsIntArray(const char* szSect, const char* szKey, int iNumInt, int* pBuf);
	bool GetValueAsFloatArray(const char* szSect, const char* szKey, int iNumFloat, float* pBuf);
	bool GetValueAsBoolean(const char* szSect, const char* szKey, bool bDefault) { return GetValueAsInt(szSect, szKey, bDefault ? 1 : 0) ? true : false; }

//	Attributes
protected:	
	bool		m_bOpened;		//	Open flag
	TConfSection*	m_pCurSect;		//	Current section
	TArray<TConfSection*>	m_sects;	//	Sections

protected:	//	Operations
	//	Parse file
	bool ParseFile(unsigned char * pBuf, unsigned char * pEnd);
	//	Get line length
	int GetLineLength(unsigned char * pLine, unsigned char * pEnd);
	//	Parse a line
	void ParseLine(unsigned char* pLine, unsigned char* pEnd);
	//	Parse key value
	bool ParseValue(TConfPair* pKey, unsigned char* pBuf, unsigned char* pEnd);

	//	Add a section
	TConfSection* AddSection(bool bComment, const TString& strName);
	//	Create a key
	TConfPair* CreateKey(const TString& strKey);

	//	Search specified value
	TString* SearchValue(const char* szSect, const char* szKey);
	//	Search specified section
	TConfSection* SearchSection(const char* szSect);
	//	Search key in specified section
	TConfPair* SearchKey(TConfSection* pSection, const char* szKey);
	//	Get specified key, create if not exist
	TConfPair* GetKey(const char* szSect, const char* szKey);
};
