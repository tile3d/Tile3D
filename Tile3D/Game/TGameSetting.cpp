#include "TGameSetting.h"


TGameSetting::TGameSetting()
{
	m_width = 800;
	m_height = 600;
	m_fullscreen = false;
}

bool TGameSetting::Init()
{
	return true;
}
