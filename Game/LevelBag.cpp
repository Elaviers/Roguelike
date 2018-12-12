#include "LevelBag.h"


LevelBag::LevelBag()
{
}


LevelBag::~LevelBag()
{
}

void LevelBag::AddLevel(const char *filename, int dominance)
{
	_levels.Add(LevelWithChance());
	_levels.Last().level.ReadFromFile(filename);
	_levels.Last().dominance = dominance;

	_totalSize += dominance;
}

const Level& LevelBag::GetLevel() const
{
	int slot = Maths::Random() * _totalSize;
	int index = 0;
	
	while (slot > _levels[index].dominance)
	{
		slot -= _levels[index].dominance;
		index++;
	}

	return _levels[index].level;
}
