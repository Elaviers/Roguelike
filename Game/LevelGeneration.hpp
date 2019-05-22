#pragma once
#include <Engine/GameObject.hpp>
#include <Engine/ObjConnector.hpp>
#include <Engine/String.hpp>

class LevelBag;

namespace LevelGeneration
{
	constexpr const char* root = "Data/Levels/";

	GameObject GenerateLevel(const String&);

}
