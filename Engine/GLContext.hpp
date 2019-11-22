#pragma once
#include "GL.hpp"
#include "Window.hpp"

class GLContext
{
private:
	HGLRC _id;

public:
	GLContext() : _id(0) {}
	GLContext(const GLContext&) = delete;
	GLContext(GLContext&& other) noexcept : _id(other._id) { other._id = 0; }
	~GLContext() {}

	void CreateDummyAndUse(LPCTSTR className);
	void Create(HDC);
	void Use(HDC) const;
	void Delete();
	
	void Create(const Window &window) { Create(window.GetHDC()); }
	void Use(const Window &window) const { Use(window.GetHDC()); }

	bool IsValid() { return _id != 0; }
};
