#pragma once
#include "Types.h"

class Timer
{
private:
	uint64 _start;
	uint64 _frequency;

public:
	Timer();
	~Timer();

	void Start();
	float SecondsSinceStart();
};
