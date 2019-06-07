#pragma once
#include "GL.hpp"
#include "Types.hpp"
#include "Vector.hpp"
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

	inline HWND GetHwnd() const { return _hwnd; }
	inline HDC GetHDC() const { return _hdc; }

	void Create(LPCTSTR className, LPCTSTR title, LPVOID param, DWORD flags = WS_OVERLAPPEDWINDOW, HWND parent = NULL);
	void Show();
	void SwapBuffers();

	inline void SetTitle(const char *title) { ::SetWindowTextA(_hwnd, title); }
	inline void SetSizeAndPos(uint16 x, uint16 y, uint16 width, uint16 height)	{ WindowFunctions::SetHWNDSizeAndPos(_hwnd, x, y, width, height); }
	inline void SetSize(uint16 width, uint16 height)							{ WindowFunctions::ResizeHWND(_hwnd, width, height); }
	inline void SetPos(uint16 x, uint16 y)										{ WindowFunctions::RepositionHWND(_hwnd, x, y); }
};