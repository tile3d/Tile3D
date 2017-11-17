#include "TGameSetting.h"


TGameSetting::TGameSetting()
{
	m_width = 800;
	m_height = 600;
	m_fullscreen = false;

	m_logLevel = 0;
	m_workDir = nullptr;
}

bool TGameSetting::Init()
{
	return true;
}
