#include "Sampler.hpp"
#include "AudioUtilities.hpp"
#include <ELCore/Utilities.hpp>
#include <ELMaths/Maths.hpp>
#include <ELSys/Debug.hpp>
#include <limits>

uint32 Sampler::ReadToSoundBuffer(byte* dest, uint32 destFrameCount, uint32 destSampleRate, uint16 destChannelCount, float mix)
{
	if (!_playing) return 0;

	if (_sound->bitsPerSample != 16) return 0;

	uint32 framesWritten = 0;

	do
	{
		uint32 offset = _currentFrame * _sound->FrameSize;

		framesWritten = AudioUtilities::ResampleTo(
			(int16*)(_sound->data + offset), _sound->sampleRate, _sound->channelCount, _sound->dataSize / _sound->FrameSize - _currentFrame,
			(int16*)dest, destSampleRate, destChannelCount, destFrameCount, mix, 3);
	} while (_loop && framesWritten < destFrameCount);

	_currentFrame += (uint32)((float)framesWritten * (float)_sound->sampleRate / (float)destSampleRate);

	return framesWritten;
}
