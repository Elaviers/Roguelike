#pragma once
#include <Engine/Buffer.h>
#include <Engine/GameObject.h>

class LevelBag
{
	struct LevelPtrWithChance
	{
		GameObject *level;
		int dominance;

		LevelPtrWithChance() : level(nullptr), dominance(0) {}
		LevelPtrWithChance(GameObject &level, int dominance) : level(&level), dominance(dominance) {}
	};

	int _totalSize;

	Buffer<LevelPtrWithChance> _levels;

public:
	LevelBag();
	~LevelBag();

	void AddLevel(GameObject &level, int dominance = 1);

	inline void RemoveLevel(const GameObject *level) 
	{
		for (uint32 i = 0; i < _levels.GetSize();)
			if (_levels[i].level == level)
				_levels.RemoveIndex(i);
			else
				++i;
	}

	const GameObject& GetNextLevel() const;
};
