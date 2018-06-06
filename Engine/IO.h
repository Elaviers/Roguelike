#pragma once
#include "Buffer.h"
#include "String.h"

struct TextureData
{
	Buffer<byte> data;
	uint32 width;
	uint32 height;

	inline bool Valid() { return data.GetSize() != 0; }
};

namespace IO {
	Buffer<byte> ReadFile(const char *filename);
	bool WriteFile(const char *filename, const byte *data, uint32 dataLength);

	String ReadFileString(const char *filename);

	TextureData ReadPNGFile(const char *filename);
}
