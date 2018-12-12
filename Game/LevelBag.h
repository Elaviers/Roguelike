#pragma once
#include <Engine/Buffer.h>
#include <Engine/Level.h>

class LevelBag
{
	struct LevelWithChance
	{
		Level level;
		int dominance;
	};

	int _totalSize;

	Buffer<LevelWithChance> _levels;

public:
	LevelBag();
	~LevelBag();

	void AddLevel(const char *filename, int dominance = 1);

	const Level& GetLevel() const;
};
