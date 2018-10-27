#include <Windows.h>
#include <CommCtrl.h>
#include "Editor.h"

//Visual styles
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int APIENTRY WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev, _In_ LPSTR cmdString, _In_ int cmdShow)
{
	::InitCommonControls();

	Editor editor;
	editor.Run();
	return 0;
}
