#pragma once
#include <Engine/Buffer.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Pair.hpp>

class Random;

class LevelBag
{
	int _totalSize;

	Buffer<Pair<GameObject*, int>> _levels;

public:
	LevelBag() : _totalSize(0) {}
	~LevelBag() {}

	void AddLevel(GameObject &level, int dominance = 1);

	inline void RemoveLevel(const GameObject *level) 
	{
		for (uint32 i = 0; i < _levels.GetSize();)
			if (_levels[i].first == level)
				_levels.RemoveIndex(i);
			else
				++i;
	}

	const GameObject& GetNextLevel(Random&) const;
};
