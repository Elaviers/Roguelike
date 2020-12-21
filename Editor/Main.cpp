#include <ELSys/Entry.hpp>
#include <Windows.h>
#include <CommCtrl.h>
#include "Editor.hpp"
#include "resource.h"

//Visual styles
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int Main()
{
	Window::Win32SetIconResource(IDI_ICON);

	::InitCommonControls();

	Editor* editor = new Editor();
	editor->Run();
	delete editor;

	return 0;
}
