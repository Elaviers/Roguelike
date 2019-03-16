#include "Sampler.h"
#include "AudioUtilities.h"
#include "Debug.h"
#include "Maths.h"
#include "Utilities.h"
#include <limits>

uint32 Sampler::ReadToSoundBuffer(byte* dest, uint32 destFrameCount, uint32 destSampleRate, uint16 destChannelCount, float mix)
{
	if (!_playing) return 0;

	if (_sound->bitsPerSample != 16) return 0;

	uint32 framesWritten = 0;

	do
	{
		framesWritten = AudioUtilities::ResampleTo(
			(int16*)(_sound->data + (_currentFrame * _sound->FrameSize)), _sound->sampleRate, _sound->channelCount, _sound->dataSize / _sound->FrameSize,
			(int16*)dest, destSampleRate, destChannelCount, destFrameCount, mix, 3);
	} while (_loop && framesWritten < destFrameCount);

	_currentFrame += framesWritten * (float)_sound->sampleRate / (float)destSampleRate;

	return framesWritten;
}
