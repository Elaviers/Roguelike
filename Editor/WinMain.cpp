#include <Windows.h>
#include <CommCtrl.h>
#include "Editor.h"

//Visual styles
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev, LPSTR cmdString, int cmdShow)
{
	::InitCommonControls();

	Editor editor;
	editor.Run();
	return 0;
}
