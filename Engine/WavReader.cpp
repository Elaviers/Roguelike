#include "IO.hpp"
#include "BufferIterator.hpp"
#include "Sound.hpp"

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
	aud.channelCount = iterator.Read_uint16_little();
	aud.sampleRate = iterator.Read_uint32_little();
	aud.byteRate = iterator.Read_uint32_little();
	aud.FrameSize = iterator.Read_uint16_little();
	aud.bitsPerSample = iterator.Read_uint16_little();
	aud.data = NULL;

	if (!(iterator.Read_byte() == 'd' && iterator.Read_byte() == 'a' && iterator.Read_byte() == 't' && iterator.Read_byte() == 'a'))
		return aud;

	aud.dataSize = iterator.Read_uint32_little();
	aud.data = new byte[aud.dataSize];

	//ReadTo is kinda slow here..
	//size_t bytesRead = iterator.ReadTo(aud.data, aud.dataSize);

	Utilities::CopyBytes(iterator.Ptr(), aud.data, aud.dataSize);

	return aud;
}
