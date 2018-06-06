#include <Windows.h>
#include "Window.h"

bool running = true;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CLOSE:
		::DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		running = false;
		break;

	default:
		return ::DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR cmdString, int cmdShow)
{
	LPCTSTR className = TEXT("WINDOWCLASS");

	WNDCLASSEX windowClass = {
		sizeof(WNDCLASSEX),
		0,
		WindowProc,
		0,
		0,
		instance,
		::LoadIcon(NULL, IDI_INFORMATION),
		::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		className,
		::LoadIcon(NULL, IDI_INFORMATION)
	};
	::RegisterClassEx(&windowClass);

	Window window;
	window.Create(className, "Window");
	window.Show();

	MSG msg;
	while (running)
	{
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		//Frame here
	}

	return 0;
}

/*
	UINT        cbSize;
	UINT        style;
	WNDPROC     lpfnWndProc;
	int         cbClsExtra;
	int         cbWndExtra;
	HINSTANCE   hInstance;
	HICON       hIcon;
	HCURSOR     hCursor;
	HBRUSH      hbrBackground;
	LPCSTR      lpszMenuName;
	LPCSTR      lpszClassName;
	HICON       hIconSm;
*/
