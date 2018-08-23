#include "Window.h"

void WindowFunctions::ResizeHWND(HWND hwnd, uint16 w, uint16 h)
{
	RECT rect;
	::GetWindowRect(hwnd, &rect);
	::MapWindowPoints(HWND_DESKTOP, ::GetParent(hwnd), (LPPOINT)&rect, 2);
	SetHWNDSizeAndPos(hwnd, rect.left, rect.top, w, h);
}

void WindowFunctions::RepositionHWND(HWND hwnd, uint16 x, uint16 y)
{
	RECT rect;
	::GetClientRect(hwnd, &rect);
	SetHWNDSizeAndPos(hwnd, x, y, rect.right, rect.bottom);
}

Window::Window() : _hwnd(NULL)
{
}


Window::~Window()
{
	if (_hwnd)
		::DestroyWindow(_hwnd);
}

void Window::Create(LPCTSTR className, LPCTSTR title, LPVOID param, DWORD flags, HWND parent)
{
	if (_hwnd)
		::DestroyWindow(_hwnd);

	_hwnd = ::CreateWindow(className, title, flags, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, parent, NULL, ::GetModuleHandle(NULL), param);

	_hdc = GetDC(_hwnd);

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,							//cColorBits
		0, 0, 0, 0, 0, 0, 0, 0,		//*
		0, 0, 0, 0, 0,				//*
		32,							//cDepthBits
		0,							//*
		0,							//*
		0,							//Ignored
		0,							//*
		0,							//*	
		0,							//*
		0							//* - Not relevant for finding PFD
	};

	int pfd_id = ChoosePixelFormat(_hdc, &pfd);
	SetPixelFormat(_hdc, pfd_id, &pfd);
}

void Window::Show()
{
	if (_hwnd)
		::ShowWindow(_hwnd, SW_SHOW);
}

void Window::SwapBuffers()
{
	::SwapBuffers(_hdc);
}
