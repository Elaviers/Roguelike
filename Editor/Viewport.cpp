#include "Viewport.h"
#include <Windows.h>

LPCTSTR Viewport::_className = TEXT("VIEWPORTCLASS");

LRESULT CALLBACK Viewport::_WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		

	default: return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

void Viewport::Initialise()
{
	WNDCLASSEXA windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.lpfnWndProc = _WindowProc;
	windowClass.hInstance = ::GetModuleHandle(NULL);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = _className;
	::RegisterClassEx(&windowClass);
}

Viewport::Viewport()
{
}


Viewport::~Viewport()
{
}
