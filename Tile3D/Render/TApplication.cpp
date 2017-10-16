#include "TApplication.h"
#include <Common/TLog.h>


bool TApplication::Init()
{
	if (!InitSetting()) {
		TLog::Log(LOG_ERR, "Application", "TApplication::Init,  Failed to init the setting");
		return false;
	}

	if (!InitLog()) {
		TLog::Log(LOG_ERR, "Application", "TApplication::Init,  Failed to init the log");
		return false;
	}

	if (!InitCmdLine()) {
		TLog::Log(LOG_ERR, "Application", "TApplication::Init,  Failed to init the cmdline");
		return false;
	}


	return true;
}

bool TApplication::InitSetting()
{
	return m_gs.Init();
}


bool TApplication::InitLog()
{
	return TLog::Init("game.log", LOG_DEBUG, false);
}

bool TApplication::InitCmdLine()
{
	return true;
}

