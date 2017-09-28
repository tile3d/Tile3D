#pragma once

class TGameSetting
{
public:
	TGameSetting();

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	bool IsFullScreen() const { return m_fullscreen; }

	bool Init();

private:
	int m_width;
	int m_height;
	bool m_fullscreen;
};

