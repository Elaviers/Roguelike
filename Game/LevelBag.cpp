#include "LevelBag.h"


LevelBag::LevelBag()
{
}


LevelBag::~LevelBag()
{
}

void LevelBag::AddLevel(GameObject &level, int dominance)
{
	_levels.Add(LevelPtrWithChance(level, dominance));
	_totalSize += dominance;
}

const GameObject& LevelBag::GetNextLevel() const
{
	int slot = Maths::Random() * _totalSize;
	int index = 0;
	
	while (slot > _levels[index].dominance)
	{
		slot -= _levels[index].dominance;
		index++;
	}

	return *_levels[index].level;
}
