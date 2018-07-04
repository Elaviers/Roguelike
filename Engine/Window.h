#pragma once
#include "GL.h"
#include "Types.h"
#include "Vector.h"
#include <Windows.h>

class Window
{
private:
	HWND	_hwnd;		//Window handle
	HDC		_hdc;		//Device context handle
	HGLRC	_hglrc;		//OpenGL rendering context handle

public:
	Window();
	virtual ~Window();

	inline HWND GetHwnd() const { return _hwnd; }
	inline HDC GetHDC() const { return _hdc; }
	inline void UseGLContext() const { wglMakeCurrent(_hdc, _hglrc); }

	void Create(LPCTSTR className, LPCTSTR title, LPVOID param, DWORD flags = WS_OVERLAPPEDWINDOW, HWND parent = NULL);
	void Show();
	void SwapBuffers();

	inline void SetTitle(const char *title) { ::SetWindowTextA(_hwnd, title); }
	inline void SetSizeAndPos(uint16 x, uint16 y, uint16 width, uint16 height) { ::SetWindowPos(_hwnd, NULL, x, y, width, height, 0); }
	void SetSize(uint16 width, uint16 height);
	void SetPos(uint16 x, uint16 y);
};
