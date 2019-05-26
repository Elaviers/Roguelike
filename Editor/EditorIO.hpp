#pragma once
#include <Engine/Buffer.hpp>
#include <Engine/IO.hpp>
#include <Engine/Pair.hpp>
#include <Engine/String.hpp>
#include <fbxsdk.h>

namespace EditorIO
{
	String OpenFileDialog(const wchar_t *dir, const Buffer<Pair<const wchar_t*>>& ext);
	String SaveFileDialog(const wchar_t *dir, const Buffer<Pair<const wchar_t*>>& ext);

	ModelData ReadFBXFile(FbxManager*, const char* filename);
}