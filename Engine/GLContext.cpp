#include "GLContext.h"
#include "Window.h"

GLContext GLContext::CreateDummyAndUse(LPCTSTR className)
{
	HWND hwnd = ::CreateWindow(className, "temp", 0, 0, 0, 0, 0, NULL, NULL, ::GetModuleHandle(NULL), 0);
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

	GLContext context;
	context.Create(hdc);
	context.Use(hdc);

	::DestroyWindow(hwnd);

	return context;
}

GLContext::GLContext() : _id(0)
{
}

GLContext::~GLContext()
{
}

void GLContext::Create(HDC hdc)
{
	_id = wglCreateContext(hdc);
}

void GLContext::Use(HDC hdc) const
{
	wglMakeCurrent(hdc, _id);
}

void GLContext::Delete()
{
	wglDeleteContext(_id);
}
