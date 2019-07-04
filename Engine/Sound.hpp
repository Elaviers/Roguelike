#pragma once
#include "Asset.hpp"
#include "Types.hpp"

enum class SoundCategory
{
	GENERIC,
	MUSIC
};

/*
	WaveSound

	WAV File
*/

class WaveSound : public Asset
{
public:
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

	virtual ~WaveSound() { delete data; }

	void MakeStereo();

	inline void Destroy() { delete[] data; }
};
