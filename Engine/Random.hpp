#pragma once
#include "Types.hpp"

class Random
{
	uint32 _rand;

	static constexpr uint32 _m = 2147483647;
	static constexpr uint32 _mEx = _m + 1;

	uint32 _PeekNext() const;

public:
	Random();
	Random(uint32 seed) : _rand(seed) {}
	~Random() {}

	uint32 Next();

	float NextFloat();

	double NextDouble()		{ return (double)Next() /	_mEx; }

	//Exclusive of max (i.e. 0 <= x < max)
	uint32 Next(uint32 max)	{ return (uint32)(NextFloat() * max); }

	//min <= x < max
	uint32 Next(uint32 min, uint32 max) { return Next(max - min) + min; }
};
