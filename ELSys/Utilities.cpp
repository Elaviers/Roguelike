#include "Utilities.hpp"
#include <ELSys/Debug.hpp>
#include <Windows.h>

String Utilities::GetSystemFontDir()
{
	char str[MAX_PATH];

	UINT length = ::GetWindowsDirectoryA(str, MAX_PATH);

	if (length > MAX_PATH) Debug::PrintLine("ERROR: (GetSystemFontDir) length returned was somehow bigger than max path");

	String fontDir(str);
	fontDir += "\\FONTS\\";

	return fontDir;
}
