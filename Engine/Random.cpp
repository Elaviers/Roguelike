#include "Random.hpp"
#include "Time.hpp"

Random::Random() : _rand(Time::GetRandSeed())
{
}

uint32 Random::Next()
{
	//Minimal random number generator
	//_m = 2147483647
	constexpr uint32 a = 16807;
	constexpr uint32 q = 127773;
	constexpr uint32 r = 2836;
	constexpr uint32 mask = 6942;

	//Mask to avoid rand being zero or a simple pattern
	_rand ^= mask;
	uint32 k = _rand / q;
	int32 newRand = a * (_rand - k * q) - r * k;
	
	if (newRand < 0) 
		newRand += _m;
	
	_rand = newRand;
	_rand ^= mask;
	return newRand;
}

float Random::NextFloat()
{
	float value = (float)Next() / _mEx;
	if (value >= 1.f)
		value = 0.9999f;

	return value;
}
