#pragma once
#include <ELSys/IO.hpp>

class Skeleton;
class WaveSound;

namespace IO
{
	WaveSound* ReadWaveFile(const char* filename);
}
