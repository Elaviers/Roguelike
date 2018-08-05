#include "Viewport.h"
#include "Editor.h"
#include <Windows.h>
#include <windowsx.h> //For X & Y LPARAM macros

LPCTSTR Viewport::_className = TEXT("VIEWPORTCLASS");

//static
LRESULT CALLBACK Viewport::_WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Viewport *viewport = (Viewport*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCT create = (LPCREATESTRUCT)lparam;
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)create->lpCreateParams);
	}
		break;

	case WM_MOUSEMOVE:
		viewport->_editor->UpdateMousePosition(viewport->_index, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));

		break;

	default: return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

//static
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
