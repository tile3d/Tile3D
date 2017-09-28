#include "TApplicationWin.h"
#include <Util/TLog.h>


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

bool TApplicationWin::Init()
{
	if (!TApplication::Init()) {
		return false;
	}

	if (!InitWindow()) {
		TLog::Log(LOG_ERR, "Application", "TApplicationWin::Init,  Failed to init the window");
		return false;
	}
	
	return true;
}

bool TApplicationWin::InitWindow()
{
	WNDCLASSEX wcex;

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_instance;
	wcex.hIcon = LoadIcon(m_instance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = "WndClassName";
	wcex.hIconSm = nullptr;

	if (!RegisterClassEx(&wcex)) {
		TLog::Log(LOG_ERR, "Application", "TApplicationWin::InitWindow,  Failed to init the register the window class");
		return false;
	}

	DWORD styles = WS_POPUP;
	if (!IsFullScreen()) {
		styles |= WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME;
	}

	DWORD exStyles = 0;
	if (IsFullScreen()) {
		exStyles = WS_EX_TOPMOST;
	}

	m_hwnd = CreateWindowEx(exStyles, "WndClassName", "Test", styles, 0, 0, GetWidth(), GetHeight(), nullptr, nullptr, m_instance, nullptr);
	if (m_hwnd == nullptr) {
		TLog::Log(LOG_ERR, "Application", "TApplicationWin::InitWindow,  Failed to create the window");
		return false;
	}

	ShowWindow(m_hwnd, m_cmdShow);
	UpdateWindow(m_hwnd);

	SetForegroundWindow(m_hwnd);
	return true;
}


