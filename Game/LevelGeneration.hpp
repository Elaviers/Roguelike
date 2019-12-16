#pragma once
#include <Engine/Entity.hpp>
#include <Engine/EntConnector.hpp>
#include <Engine/String.hpp>

class LevelBag;

namespace LevelGeneration
{
	constexpr const char* ROOT_DIR = "Data/Levels/";

	bool GenerateLevel(Entity& root, const String&);

}
