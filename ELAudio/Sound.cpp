#include "Sound.hpp"
#include <ELCore/Utilities.hpp>

void WaveSound::MakeStereo()
{
	if (this->channelCount == 2) return;

	if (this->channelCount == 1)
	{
		uint32 newSize = this->dataSize * 2;
		byte* newData = new byte[newSize];

		byte sampleSize = this->bitsPerSample / 8;

		for (uint32 i = 0; i < this->dataSize; i += sampleSize) {
			Utilities::CopyBytes(&this->data[i], &newData[i * 2], sampleSize);
			Utilities::CopyBytes(&this->data[i], &newData[(i * 2) + sampleSize], sampleSize);
		}

		this->data = newData;
		this->dataSize = newSize;
		this->channelCount = 2;
		this->FrameSize *= 2;
		this->byteRate *= 2;
	}

}
