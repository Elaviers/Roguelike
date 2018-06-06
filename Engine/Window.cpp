#include "Window.h"

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

	_hglrc = wglCreateContext(_hdc);
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

void Window::SetSize(int width, int height)
{
	RECT rect;
	::GetWindowRect(_hwnd, &rect);

	SetSizeAndPos(rect.left, rect.top, width, height);
}

void Window::SetPos(int x, int y)
{
	RECT rect;
	::GetClientRect(_hwnd, &rect);

	SetSizeAndPos(x, y, rect.right, rect.bottom);
}
