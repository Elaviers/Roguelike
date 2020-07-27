#include "Debug.hpp"
#include <ELCore/String.hpp>
#include <Windows.h>

namespace Debug
{
	void Break()
	{
		::DebugBreak();
	}

	void Print(const char* string)
	{
		::OutputDebugStringA(string);
	}

	void PrintLine(const char *string)
	{
		::OutputDebugStringA(CSTR(string, '\n'));
	}

	void Error(const char *string)
	{
		PrintLine(CSTR("ERROR: ", string));
		Message(string, "Error");
	}

	void FatalError(const char *string)
	{
		PrintLine(CSTR("FATAL ERROR! ", '\"', string, '\"'));
		Message(string, "Fatal Error");

		Break();

		::ExitProcess(1);
	}

	void Message(const char* message, const char* title)
	{
		::MessageBoxA(NULL, message, title, MB_OK);
	}
}
