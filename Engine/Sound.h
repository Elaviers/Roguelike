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
	uint16 channel_count;
	uint32 sample_rate;
	uint32 byte_rate;
	uint16 block_align;			//bytes per sample
	uint16 bits_per_sample;

	byte* data;
	uint32 data_size;

	float volume;
	SoundCategory category;

	~WaveSound() { }

	void MakeStereo();

	inline void Destroy() { delete data; }
};
