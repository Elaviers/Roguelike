#pragma once
#include <Engine/Buffer.hpp>
#include <Engine/Pair.hpp>
#include <Engine/String.hpp>

namespace EditorIO
{
	String OpenFileDialog(const wchar_t *dir, const Buffer<Pair<const wchar_t*>>& ext);
	String SaveFileDialog(const wchar_t *dir, const Buffer<Pair<const wchar_t*>>& ext);
}