#pragma once
#include <Engine/GameObject.h>
#include <Engine/ObjConnector.h>
#include <Engine/String.h>

class LevelBag;

namespace LevelGeneration
{
	constexpr const char* root = "Data/Levels/";

	GameObject GenerateLevel(const String&);

}
