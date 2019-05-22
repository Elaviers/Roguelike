#include "ToolWindow.hpp"
#include <CommCtrl.h>

LPCTSTR ToolWindow::_className = TEXT("TOOLPROPERTIESCLASS");


LRESULT ToolWindow::_WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	ToolWindow *tp = (ToolWindow*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCT create = (LPCREATESTRUCT)lparam;
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)create->lpCreateParams);

		::CreateWindowEx(0, WC_BUTTON, "Tool Properties", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 4, 4, 248, 248, hwnd, NULL, ::GetModuleHandle(NULL), NULL);
	}
		break;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

void ToolWindow::Initialise(HBRUSH brush)
{
	WNDCLASSEXA windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.lpfnWndProc = _WindowProc;
	windowClass.hInstance = ::GetModuleHandle(NULL);
	windowClass.hbrBackground = brush;
	windowClass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = _className;
	::RegisterClassEx(&windowClass);
}
