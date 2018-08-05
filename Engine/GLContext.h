#pragma once
#include "GL.h"

class Window;

class GLContext
{
private:
	HGLRC _id;

public:
	GLContext();
	~GLContext();

	void Create(const Window&);
	void Use(const Window&);
};
