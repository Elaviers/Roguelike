#pragma once
#include <Engine/String.h>

namespace EditorIO
{
	String OpenFileDialog(const char *dir, const char *ext);
	String SaveFileDialog(const char *dir, const char *ext);
}