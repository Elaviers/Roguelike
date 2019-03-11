#include "IO.h"
#include "BufferIterator.h"

WaveSound IO::ReadWaveFile(const char* filename)
{
	Buffer<byte> data = IO::ReadFile(filename);
	BufferIterator<byte> iterator(data);

	WaveSound aud = {0};

	if (!(iterator.Read_byte() == 'R' && iterator.Read_byte() == 'I' && iterator.Read_byte() == 'F' && iterator.Read_byte() == 'F'))
		return aud;

	uint32 chunkSize = iterator.Read_uint32_little();

	if (!(iterator.Read_byte() == 'W' && iterator.Read_byte() == 'A' && iterator.Read_byte() == 'V' && iterator.Read_byte() == 'E'))
		return aud;

	if (!(iterator.Read_byte() == 'f' && iterator.Read_byte() == 'm' && iterator.Read_byte() == 't' && iterator.Read_byte() == ' '))
		return aud;

	uint32 fmtChunkSize = iterator.Read_uint32_little();
	aud.format = iterator.Read_uint16_little();
	aud.channel_count = iterator.Read_uint16_little();
	aud.sample_rate = iterator.Read_uint32_little();
	aud.byte_rate = iterator.Read_uint32_little();
	aud.block_align = iterator.Read_uint16_little();
	aud.bits_per_sample = iterator.Read_uint16_little();
	aud.data = NULL;

	if (!(iterator.Read_byte() == 'd' && iterator.Read_byte() == 'a' && iterator.Read_byte() == 't' && iterator.Read_byte() == 'a'))
		return aud;

	aud.data_size = iterator.Read_uint32_little();
	aud.data = new byte[aud.data_size];

	size_t bytesRead = iterator.ReadTo(aud.data, aud.data_size);

	return aud;
}
