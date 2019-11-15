#include "LevelBag.hpp"
#include <Engine/Random.hpp>

void LevelBag::AddLevel(Entity &level, int dominance)
{
	_levels.Add(Pair<Entity*, int>(&level, dominance));
	_totalSize += dominance;
}

const Entity& LevelBag::GetNextLevel(Random &random) const
{
	int slot = (int)random.Next(_totalSize);
	int index = 0;
	
	while (slot > _levels[index].second)
	{
		slot -= _levels[index].second;
		index++;
	}

	return *_levels[index].first;
}
