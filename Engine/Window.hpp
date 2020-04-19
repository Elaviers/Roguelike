#pragma once
#include "GL.hpp"
#include "Types.hpp"
#include <Windows.h>

namespace WindowFunctions
{
	inline void SetHWNDSizeAndPos(HWND hwnd, uint16 x, uint16 y, uint16 w, uint16 h) { ::SetWindowPos(hwnd, NULL, x, y, w, h, 0); }
	void ResizeHWND(HWND, uint16 w, uint16 h);
	void RepositionHWND(HWND, uint16 x, uint16 y);

	void SetDefaultPixelFormat(HDC hdc);
}

class Window
{
protected:
	HWND	_hwnd;		//Window handle
	HDC		_hdc;		//Device context handle

public:
	Window() : _hwnd(NULL), _hdc(NULL) { }

	virtual ~Window()
	{
		if (_hwnd)
			::DestroyWindow(_hwnd);
	}

	HWND GetHwnd() const { return _hwnd; }
	HDC GetHDC() const { return _hdc; }

	void Create(LPCTSTR className, LPCTSTR title, LPVOID param, DWORD flags = WS_OVERLAPPEDWINDOW, HWND parent = NULL);
	void Show();
	void Hide();
	void SwapBuffers();

	void Focus();

	void SetTitle(const char *title) { ::SetWindowTextA(_hwnd, title); }
	void SetSizeAndPos(uint16 x, uint16 y, uint16 width, uint16 height)	{ WindowFunctions::SetHWNDSizeAndPos(_hwnd, x, y, width, height); }
	void SetSize(uint16 width, uint16 height)							{ WindowFunctions::ResizeHWND(_hwnd, width, height); }
	void SetPos(uint16 x, uint16 y)										{ WindowFunctions::RepositionHWND(_hwnd, x, y); }
};
