#pragma once
#include "FbxSdk.hpp"
#include <Engine/Animation.hpp>
#include <Engine/Buffer.hpp>
#include <Engine/IO.hpp>
#include <Engine/Pair.hpp>
#include <Engine/String.hpp>

namespace EditorIO
{
	String OpenFileDialog(const wchar_t *dir, const Buffer<Pair<const wchar_t*>>& ext);
	String SaveFileDialog(const wchar_t *dir, const Buffer<Pair<const wchar_t*>>& ext);

	Mesh* ReadFBXMesh(FbxManager*, const char* filename);
	Animation* ReadFBXAnimation(FbxManager*, const char* filename, Skeleton&);
}