#include "LevelBag.hpp"
#include <Engine/Random.hpp>

void LevelBag::AddLevel(GameObject &level, int dominance)
{
	_levels.Add(Pair<GameObject*, int>(&level, dominance));
	_totalSize += dominance;
}

const GameObject& LevelBag::GetNextLevel(Random &random) const
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
