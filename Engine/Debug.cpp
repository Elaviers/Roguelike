#include "Debug.h"
#include "String.h"
#include <Windows.h>

namespace Debug
{
	void Break()
	{
		::DebugBreak();
	}

	void PrintLine(const char *string)
	{
		::OutputDebugStringA(CSTR(string + '\n'));
	}

	void Error(const char *string)
	{
		PrintLine(CSTR("ERROR: " + string));
		::MessageBoxA(NULL, string, "Error", MB_OK);
	}

	void FatalError(const char *string)
	{
		PrintLine(CSTR("FATAL ERROR! " + '\"'  + string + '\"'));
		::MessageBoxA(NULL, string, "Fatal Error", MB_OK);

		Break();

		::ExitProcess(1);
	}
}