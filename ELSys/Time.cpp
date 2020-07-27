#include "Time.hpp"
#include <Windows.h>

uint32 Time::GetRandSeed()
{
	SYSTEMTIME time;
	::GetSystemTime(&time);

	return (uint32)time.wMilliseconds * time.wSecond * time.wHour;
}
