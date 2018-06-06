#pragma once
#include <Engine/Window.h>
#include <Engine/Camera.h>
#include <Windows.h>

class Viewport : public Window
{
private:
	Window _window;

	static LPCTSTR _className;

	static LRESULT _WindowProc(HWND, UINT, WPARAM, LPARAM);

public:
	Viewport();
	~Viewport();

	inline void Create(HWND parent) { Window::Create(_className, nullptr, NULL, WS_CHILD, parent); }

	static void Initialise();
};
