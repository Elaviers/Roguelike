#pragma once
#include "Types.h"

enum class SoundCategory
{
	GENERIC,
	MUSIC
};

/*
	WaveSound

	WAV File
*/

struct WaveSound
{
	uint16 format;
	uint16 channelCount;
	uint32 sampleRate;
	uint32 byteRate;
	uint16 FrameSize;
	uint16 bitsPerSample;

	byte* data;
	uint32 dataSize;

	float volume;
	SoundCategory category;

	~WaveSound() { }

	void MakeStereo();

	inline void Destroy() { delete[] data; }
};
