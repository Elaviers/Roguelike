#pragma once
#include "FbxSdk.hpp"
#include <Engine/Animation.hpp>
#include <Engine/Mesh.hpp>

namespace EditorIO
{
	Mesh* ReadFBXMesh(FbxManager*, const char* filename);
	Animation* ReadFBXAnimation(FbxManager*, const char* filename, const Skeleton&);
}