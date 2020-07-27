#pragma once
#include <ELCore/String.hpp>
#include <Engine/Entity.hpp>
#include <Engine/EntConnector.hpp>

class LevelBag;

namespace LevelGeneration
{
	constexpr const char* ROOT_DIR = "Data/Levels/";

	bool GenerateLevel(Entity& root, const String&, const Context&);

}
