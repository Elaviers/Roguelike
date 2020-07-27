#include "GLContext.hpp"
#include <ELCore/String.hpp>
#include "Debug.hpp"
#include "Window.hpp"

void GLContext::_Create(HDC hdc)
{
	_id = wglCreateContext(hdc);

	if (_id == NULL)
	{
		DWORD error = ::GetLastError();
		Debug::FatalError(CSTR("wglCreateContext failed (0x", String::FromInt(error, 2, 16), ")"));
	}
}

void GLContext::_Use(HDC hdc) const
{
	wglMakeCurrent(hdc, _id);
}

void GLContext::CreateDummyAndUse()
{
	Window::_EnsureWindowClasses();
	HWND hwnd = ::CreateWindow(Window::_wclassDummy, "temp", 0, 0, 0, 0, 0, NULL, NULL, Window::_programInstance, 0);
	HDC hdc = ::GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pfd = { 0 };
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;

	int pfdId = ::ChoosePixelFormat(hdc, &pfd);

	::SetPixelFormat(hdc, pfdId, &pfd);

	_Create(hdc);
	_Use(hdc);

	::DestroyWindow(hwnd);
}

void GLContext::Delete()
{
	wglDeleteContext(_id);
}
