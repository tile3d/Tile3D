#pragma once


//
//1) display setting(width/height/dpi/)
//2) rendering setting()
//3) shortcut setting
//4) misc(loglevel, workdir)
//
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

	int m_logLevel;
	char* m_workDir;
};

