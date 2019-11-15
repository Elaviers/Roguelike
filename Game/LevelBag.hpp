#pragma once
#include <Engine/Buffer.hpp>
#include <Engine/Entity.hpp>
#include <Engine/Pair.hpp>

class Random;

class LevelBag
{
	int _totalSize;

	Buffer<Pair<Entity*, int>> _levels;

public:
	LevelBag() : _totalSize(0) {}
	~LevelBag() {}

	void AddLevel(Entity &level, int dominance = 1);

	inline void RemoveLevel(const Entity *level) 
	{
		for (uint32 i = 0; i < _levels.GetSize();)
			if (_levels[i].first == level)
				_levels.RemoveIndex(i);
			else
				++i;
	}

	const Entity& GetNextLevel(Random&) const;
};
