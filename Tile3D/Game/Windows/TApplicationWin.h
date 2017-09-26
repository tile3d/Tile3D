#pragma once

#include <Game/TApplication.h>
#include <Windows.h>

class TApplicationWin : public TApplication
{
public:
	TApplicationWin(HINSTANCE ins, int cmdShow) {
		m_instance = ins;
		m_cmdShow = cmdShow;
	}

	bool Init();

	bool InitWindow();

	HWND & GetWnd() { return m_hwnd; }

private:
	HINSTANCE m_instance;
	HWND m_hwnd;
	int m_cmdShow;
};

