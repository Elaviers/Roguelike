#pragma once
#include "Window.hpp"

//Just a window with exposed Win32 stuff

#ifdef WIN32

class Window_Win32 : public Window
{

public:
	Window_Win32() {}
	virtual ~Window_Win32() {}

	void Create(LPCTSTR className, LPCTSTR title, LPVOID param, DWORD flags = WS_OVERLAPPEDWINDOW, HWND parent = NULL);

	HWND GetHWND() const { return _hwnd; }
	HDC GetHDC() const { return _hdc; }
};

#endif
