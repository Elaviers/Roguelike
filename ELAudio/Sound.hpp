#pragma once
#include <ELCore/Asset.hpp>
#include <ELCore/Types.hpp>

enum class ESoundCategory
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
	uint16 format = 0;
	uint16 channelCount = 0;
	uint32 sampleRate = 0;
	uint32 byteRate = 0;
	uint16 FrameSize = 0;
	uint16 bitsPerSample = 0;

	byte* data = nullptr;
	uint32 dataSize = 0;

	float volume = 0.f;
	ESoundCategory category = ESoundCategory::GENERIC;

	virtual ~WaveSound() { delete data; }

	void MakeStereo();

	void Destroy() { delete[] data; }
};
