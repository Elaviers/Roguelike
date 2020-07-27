#pragma once
#include <ELCore/Types.hpp>

namespace AudioUtilities
{
	void LowPassFilter(const int16* src, int16* dest, int channelCount, size_t frameCount, uint32 sampleRate, uint32 cutoff);

	float LanczosInterp(const int16* src, int channelCount, size_t frameCount, uint32 index, float blend, unsigned int filterSize);

	//Returns dest frames written
	uint32 ResampleTo(int16* src, uint32 srcSampleRate, uint16 srcChannelCount, uint32 srcFrameCount,
		int16* dest, uint32 destSampleRate, uint16 destChannelCount, uint32 destFrameCount, float mix = 1.f, unsigned int filterSize = 3);
}
