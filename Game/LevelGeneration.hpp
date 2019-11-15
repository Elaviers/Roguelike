#pragma once
#include <Engine/Entity.hpp>
#include <Engine/EntConnector.hpp>
#include <Engine/String.hpp>

class LevelBag;

namespace LevelGeneration
{
	constexpr const char* root = "Data/Levels/";

	Entity GenerateLevel(const String&);

}
