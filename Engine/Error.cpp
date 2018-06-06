#include "Error.h"
#include <Windows.h>

void Error(const char *string)
{
	::MessageBoxA(NULL, string, "Error", MB_OK);
}

void FatalError(const char *string)
{
	::MessageBoxA(NULL, string, "Fatal Error", MB_OK);
	::ExitProcess(1);
}
