#include "AudioUtilities.hpp"
#include <ELMaths/Maths.hpp>
#include <limits>

void AudioUtilities::LowPassFilter(const int16 *src, int16 *dest, int channelCount, size_t frameCount, uint32 sampleRate, uint32 cutoff)
{
	if (frameCount > 0)
	{
		float rc = 1.f / ((float)cutoff * 2.f * (float)Maths::PI);
		float dt = 1.f / sampleRate;
		float alpha = dt / (rc + dt);

		for (int c = 0; c < channelCount; ++c)
			dest[c] = src[c];

		for (size_t f = 1; f < frameCount; ++f)
		{
			for (int c = 0; c < channelCount; ++c)
			{
				size_t index = f * channelCount + c;
				size_t prevIndex = index - channelCount;

				dest[index] = dest[prevIndex] + (uint32)(alpha * (src[index] - dest[prevIndex]));
			}
		}
	}
}

float LanczosKernel(float x, unsigned int filterSize)
{
	if (x == 0.f)
		return 1.f;

	if (x < filterSize && x >= -(int)filterSize)
	{
		return ((float)filterSize * Maths::Sine(Maths::PI_F * x) * Maths::Sine(Maths::PI_F * x / (float)filterSize)) / (Maths::PI_F * Maths::PI_F * x * x);
	}

	return 0.f;
}

float AudioUtilities::LanczosInterp(const int16* src, int channelCount, size_t frameCount, uint32 index, float blend, unsigned int filterSize)
{
	if (blend == 0.f)
		return src[index];

	const int fs = filterSize;

	int32 start = -fs + 1;

	if ((int)index + (start * channelCount) < 0)
		start = index / channelCount * -1;

	float result = 0.f;

	for (int32 i = start; i <= fs; ++i)
		result += src[index + i * channelCount] * LanczosKernel(blend - i, filterSize);

	return result;
}

uint32 AudioUtilities::ResampleTo(int16 *src, uint32 srcSampleRate, uint16 srcChannelCount, uint32 srcFrameCount,
							  int16 *dest, uint32 destSampleRate, uint16 destChannelCount, uint32 destFrameCount, float mix, unsigned int filterSize)
{
	const float srcFramesPerDestFrame = (float)srcSampleRate / (float)destSampleRate;
	const float destFramesPerSrcFrame = (float)destSampleRate / (float)srcSampleRate;

	uint32 framesToWrite = Maths::Min<uint32>((uint32)((float)srcFrameCount * destFramesPerSrcFrame), destFrameCount);

	for (uint32 f = 0; f < framesToWrite; ++f)
	{
		//t is in src frames
		float t = f * srcFramesPerDestFrame;

		uint32 srcFrame = (uint32)t;

		uint32 srcIndex = srcFrame * srcChannelCount;
		float blend = t - srcFrame;

		for (int c = 0; c < srcChannelCount; ++c)
		{
			uint32 destIndex = f * destChannelCount + c;

			float amount = dest[destIndex] + mix * AudioUtilities::LanczosInterp(src, srcChannelCount, srcFrameCount, srcIndex + c, blend, filterSize);

			dest[destIndex] = (int16)Maths::Clamp<float>(amount, std::numeric_limits<int16>::min(), std::numeric_limits<int16>::max());
		}
	}

	return framesToWrite;
}
