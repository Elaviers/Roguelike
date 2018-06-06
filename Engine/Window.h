#pragma once
#include "GL.h"
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
	inline void SetSizeAndPos(int x, int y, int width, int height) { ::SetWindowPos(_hwnd, NULL, x, y, width, height, 0); }
	void SetSize(int width, int height);
	void SetPos(int x, int y);
};
