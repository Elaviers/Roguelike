#pragma once
#include <Engine/Buffer.h>
#include <Engine/Pair.h>
#include <Engine/String.h>

namespace EditorIO
{
	String OpenFileDialog(const wchar_t *dir, const Buffer<Pair<const wchar_t*>>& ext);
	String SaveFileDialog(const wchar_t *dir, const Buffer<Pair<const wchar_t*>>& ext);
}