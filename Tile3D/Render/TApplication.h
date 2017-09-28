#pragma once

#include "TGameSetting.h"

class TApplication
{
public:
	virtual bool Init();

	int GetWidth() { return m_gs.GetWidth(); }
	int GetHeight() { return m_gs.GetHeight(); }
	bool IsFullScreen() { return m_gs.IsFullScreen(); }

private:
	bool InitSetting();
	bool InitLog();
	bool InitCmdLine();

	//TBD
	bool InitFileDir();
	bool InitPCKFile();
	bool InitNetwork();


private:
	TGameSetting m_gs;

};

