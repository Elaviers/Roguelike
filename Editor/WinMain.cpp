#include <Windows.h>
#include "Editor.h"

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev, LPSTR cmdString, int cmdShow)
{
	Editor editor;
	editor.Run();
	return 0;
}
