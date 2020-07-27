#include "Random.hpp"

uint32 Random::_PeekNext() const
{
	//Minimal random number generator
	//_m = 2147483647
	constexpr uint32 a = 16807;
	constexpr uint32 q = 127773;
	constexpr uint32 r = 2836;
	constexpr uint32 mask = 6942;

	//Mask to avoid rand being zero or a simple pattern
	uint32 newRand = _rand ^ mask;
	uint32 k = newRand / q;
	newRand = a * (newRand - k * q) - r * k;

	if (newRand < 0)
		newRand += _m;

	return newRand;
}

uint32 Random::Next()
{
	constexpr uint32 mask = 6942;

	uint32 newRand = _PeekNext();
	_rand = newRand ^ mask;
	return newRand;
}

float Random::NextFloat()
{
	float value = (float)Next() / _mEx;
	if (value >= 1.f)
		value = 0.9999f;

	return value;
}
