#include "Utilities.hpp"
#include "Debug.hpp"
#include <Windows.h>

String Utilities::GetSystemFontDir()
{
	String fontDir;
	fontDir.SetLength(MAX_PATH);

	UINT length = ::GetWindowsDirectoryA(&fontDir[0], (UINT)fontDir.GetLength());

	if (length > MAX_PATH) Debug::PrintLine("ERROR: (GetSystemFontDir) length returned was somehow bigger than max path");

	fontDir.Trim();
	fontDir += "\\FONTS\\";

	return fontDir;
}
