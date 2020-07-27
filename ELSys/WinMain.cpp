#include <Windows.h>
#include "Debug.hpp"
#include "Entry.hpp"
#include "Window.hpp"

int APIENTRY WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev, _In_ LPSTR cmdString, _In_ int cmdShow)
{
	Window::_programInstance = instance;

	RAWINPUTDEVICE rawMouseInput;
	rawMouseInput.usUsagePage = 0x01;
	rawMouseInput.usUsage = 0x02;
	rawMouseInput.dwFlags = 0;
	rawMouseInput.hwndTarget = 0;

	if (RegisterRawInputDevices(&rawMouseInput, 1, sizeof(rawMouseInput)) == FALSE)
	{
		Debug::Error("Raw mouse input registration error!");
	}

	return Main();
}
