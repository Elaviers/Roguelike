#pragma once
#include <ELCore/String.hpp>
#include <Engine/Entity.hpp>
#include <Engine/EntConnector.hpp>

class LevelBag;
class World;

namespace LevelGeneration
{
	constexpr const char* ROOT_DIR = "Data/Levels/";

	bool GenerateLevel(World& root, const String&, const Context&);

}
