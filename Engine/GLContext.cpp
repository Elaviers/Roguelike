#include "GLContext.h"
#include "Window.h"

GLContext::GLContext() : _id(0)
{
}

GLContext::~GLContext()
{
	wglDeleteContext(_id);
}

void GLContext::Create(const Window &window)
{
	_id = wglCreateContext(window.GetHDC());
}

void GLContext::Use(const Window &window)
{
	wglMakeCurrent(window.GetHDC(), _id);
}
