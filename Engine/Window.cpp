#include "Window.hpp"

void WindowFunctions::ResizeHWND(HWND hwnd, uint16 w, uint16 h)
{
	RECT _rect;
	::GetWindowRect(hwnd, &_rect);
	::MapWindowPoints(HWND_DESKTOP, ::GetParent(hwnd), (LPPOINT)&_rect, 2);
	SetHWNDSizeAndPos(hwnd, (uint16)_rect.left, (uint16)_rect.top, w, h);
}

void WindowFunctions::RepositionHWND(HWND hwnd, uint16 x, uint16 y)
{
	RECT _rect;
	::GetClientRect(hwnd, &_rect);
	SetHWNDSizeAndPos(hwnd, x, y, (uint16)_rect.right, (uint16)_rect.bottom);
}

void WindowFunctions::SetDefaultPixelFormat(HDC hdc)
{
	const int attribInts[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLE_BUFFERS_ARB, 1,
		WGL_SAMPLES_ARB, 8,					//8x MSAA
		0, // End
	};

	int pfdId;
	UINT formatCount;

	wglChoosePixelFormatARB(hdc, attribInts, NULL, 1, &pfdId, &formatCount);
	
	PIXELFORMATDESCRIPTOR pfd = {0};
	DescribePixelFormat(hdc, pfdId, sizeof(pfd), &pfd);
	//Get a pfd so Windows doesn't cry

	SetPixelFormat(hdc, pfdId, &pfd);
}

void Window::Create(LPCTSTR className, LPCTSTR title, LPVOID param, DWORD flags, HWND parent)
{
	if (_hwnd)
		::DestroyWindow(_hwnd);

	_hwnd = ::CreateWindow(className, title, flags, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, parent, NULL, ::GetModuleHandle(NULL), param);

	_hdc = GetDC(_hwnd);
	WindowFunctions::SetDefaultPixelFormat(_hdc);
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
