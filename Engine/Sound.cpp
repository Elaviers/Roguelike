#include "Sound.h"
#include "Utilities.h"

void WaveSound::MakeStereo()
{
	if (this->channel_count == 2) return;

	if (this->channel_count == 1)
	{
		uint32 newSize = this->data_size * 2;
		byte* newData = new byte[newSize];

		byte sampleSize = this->bits_per_sample / 8;

		for (uint32 i = 0; i < this->data_size; i += sampleSize) {
			Utilities::CopyBytes(&this->data[i], &newData[i * 2], sampleSize);
			Utilities::CopyBytes(&this->data[i], &newData[(i * 2) + sampleSize], sampleSize);
		}

		this->data = newData;
		this->data_size = newSize;
		this->channel_count = 2;
		this->block_align *= 2;
		this->byte_rate *= 2;
	}

}
