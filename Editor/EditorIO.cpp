#include "EditorIO.h"
#include <Windows.h>

inline void InitialiseOpenFile(OPENFILENAMEA &ofn, const char *filter, const char *dir, char *filename)
{
	ofn.lStructSize = sizeof(OPENFILENAMEA);

	ofn.lpstrFilter = filter;

	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;

	ofn.lpstrInitialDir = dir;
}

String EditorIO::OpenFileDialog(const char *dir, const char *filter)
{
	char filename[MAX_PATH] = "";

	OPENFILENAMEA ofn = {};
	::InitialiseOpenFile(ofn, filter, dir, filename);

	if (::GetOpenFileNameA(&ofn))
		return filename;

	return "";
}

String EditorIO::SaveFileDialog(const char *dir, const char *filter)
{
	char filename[MAX_PATH] = "";

	OPENFILENAMEA ofn = {};
	::InitialiseOpenFile(ofn, filter, dir, filename);

	if (::GetSaveFileNameA(&ofn))
		return filename;

	return "";
}

