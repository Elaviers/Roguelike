#include "GLContext.h"
#include "Window.h"

GLContext::GLContext() : _id(0)
{
}

GLContext::~GLContext()
{
	wglDeleteContext(_id);
}

void GLContext::Create(HDC hdc)
{
	_id = wglCreateContext(hdc);
}

void GLContext::Use(HDC hdc) const
{
	wglMakeCurrent(hdc, _id);
}
