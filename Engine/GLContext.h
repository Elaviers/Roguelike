#pragma once
#include "GL.h"
#include "Window.h"

class GLContext
{
private:
	HGLRC _id;

public:
	GLContext();
	~GLContext();

	void Create(HDC);
	void Use(HDC) const;

	inline void Create(const Window &window) { Create(window.GetHDC()); }
	inline void Use(const Window &window) const { Use(window.GetHDC()); }
};
