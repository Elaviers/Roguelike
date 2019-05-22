#pragma once
#include "GL.hpp"
#include "Window.hpp"

class GLContext
{
private:
	HGLRC _id;

public:
	GLContext();
	~GLContext();

	void Create(HDC);
	void Use(HDC) const;
	void Delete();
	
	inline void Create(const Window &window) { Create(window.GetHDC()); }
	inline void Use(const Window &window) const { Use(window.GetHDC()); }

	inline bool IsValid() { return _id != 0; }

	static GLContext CreateDummyAndUse(LPCTSTR className);
};
