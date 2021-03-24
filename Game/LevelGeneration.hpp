#pragma once
#include <ELCore/String.hpp>
#include <Engine/WorldObject.hpp>
#include <Engine/OConnector.hpp>

class LevelBag;
class World;

namespace LevelGeneration
{
	constexpr const char* ROOT_DIR = "Data/Levels/";

	bool GenerateLevel(World& root, const String&);

}
