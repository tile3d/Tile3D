#pragma once

class TFile;
class TScriptFile
{
public:		
	enum
	{
		MAX_LINELEN = 2048		//	Maximum line length in characters
	};

	struct ScriptInfo
	{
		char*	pStart;		//	Start address of buffer
		char*	pEnd;		//	End address of buffer
		char*	pCur;		//	Current pointer
		int		iLine;		//	Line counter
	};

public:
	TScriptFile();
	virtual ~TScriptFile();

	//	Open an existing script file
	bool Open(TFile* pFile);
	bool Open(const char* szFile);
	//	Close file
	void Close();

	//	Reset pointers
	void ResetScriptFile();
	//	Get next token and move file pointer forward
	bool GetNextToken(bool bCrossLine);
	//	Peek next token and don't move file pointer
	bool PeekNextToken(bool bCrossLine);
	//	Skip current line
	bool SkipLine();
	//	Search specified token
	bool MatchToken(char* szToken, bool bCaseSensitive);

	//	Get next token as float
	inline float GetNextTokenAsFloat(bool bCrossLine);
	//	Get next token as int
	inline int GetNextTokenAsInt(bool bCrossLine);

	//	Reach end of file ?
	bool IsEnd() { return m_script.pCur >= m_script.pEnd; }
	//	Get current line
	int GetCurLine() { return m_script.iLine; }

//	Attributes
public:		
	char	m_token[MAX_LINELEN];
	ScriptInfo	m_script;		//	Script file information
};

