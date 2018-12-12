#pragma once
#include <Engine/Collection.h>
#include <Engine/Connector.h>
#include <Engine/String.h>

class LevelBag;

namespace LevelGeneration
{
	constexpr const char* root = "Data/Levels/";

	void GenerateSegment(const LevelBag &bag, const Connector &connector, Collection &collection, int depth);

	Collection GenerateLevel(const String&);


}
