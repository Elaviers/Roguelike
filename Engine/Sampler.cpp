#include "Sampler.h"
#include "Maths.h"

uint32 Sampler::ReadToSoundBuffer(byte* dest, uint32 destFrameCount, uint32 destSampleRate, uint16 destChannelCount, float mix)
{
	if (!_playing) return 0;

	uint32 framesToWrite = 0;

	const float srcFramesPerDestFrame = (float)_sound->sample_rate / (float)destSampleRate;
	const float destFramesPerSrcFrame = (float)destSampleRate / (float)_sound->sample_rate;

	if (_sound->bits_per_sample == 16)
	{
		int16* const dest16 = reinterpret_cast<int16*>(dest);
		int16* const srcData = (int16*)_sound->data;

		//dest frames that we will write 
		framesToWrite = (uint32)((float)((_sound->data_size / _sound->block_align) - _currentFrame) * destFramesPerSrcFrame);
		if (destFrameCount < framesToWrite)
			framesToWrite = destFrameCount;

		uint16 srcChannelCount = _sound->channel_count;
		float firstSrcIndex = (float)(_currentFrame * srcChannelCount);

		//i is the FRAME INDEX
		for (uint32 i = 0; i < framesToWrite; ++i) {
			//t = first read frame + i src frames
			float t = firstSrcIndex + ((i * srcChannelCount) * srcFramesPerDestFrame);
			uint32 src_frame_index = (uint32)t;
			float alpha = t - (float)src_frame_index;

			for (uint16 channel = 0; channel < destChannelCount; ++channel) {
				//dest sample
				uint32 dest_index = (i * destChannelCount) + channel;

				//amount = dest sample + mixed src sample at time
				float amount;

				if (i + 1 < framesToWrite)
					amount = (float)dest16[dest_index] + (mix * Maths::Lerp(srcData[src_frame_index + channel], srcData[src_frame_index + channel + srcChannelCount], alpha));
				else
					amount = (float)dest16[dest_index] + (mix * srcData[src_frame_index + channel]);

				if (amount > 32767.f)
					amount = 32767.f;
				else if (amount < -32768.f)
					amount = -32768.f;

				dest16[dest_index] = (int16)amount;
			}
		}
	}

	_currentFrame += (uint32)(framesToWrite * srcFramesPerDestFrame);

	if (_loop && framesToWrite < destFrameCount) {
		_currentFrame = 0;
		framesToWrite += ReadToSoundBuffer(dest, destFrameCount - framesToWrite, destSampleRate, destChannelCount, mix);
	}

	return framesToWrite;
}
