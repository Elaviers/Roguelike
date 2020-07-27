#include <ELSys/Entry.hpp>
#include <Windows.h>
#include <CommCtrl.h>
#include "Editor.hpp"

//Visual styles
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int Main()
{
	::InitCommonControls();

	{
		Editor editor;
		editor.Run();
	}

	return 0;
}
