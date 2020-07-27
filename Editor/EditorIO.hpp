#pragma once
#include "FbxSdk.hpp"
#include <ELGraphics/Animation.hpp>

class Mesh;

namespace EditorIO
{
	Mesh* ReadFBXMesh(FbxManager*, const char* filename);
	Animation* ReadFBXAnimation(FbxManager*, const char* filename, const Skeleton&);
}