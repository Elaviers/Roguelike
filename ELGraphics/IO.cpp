#pragma once
#include "IO.hpp"
#include "lodepng.h"

bool IO::ReadPNGFile(const Buffer<byte>& data, Buffer<byte>& outData, unsigned int& outWidth, unsigned int& outHeight)
{
	unsigned char* buffer;
	unsigned int width, height;

	lodepng_decode_memory(&buffer, &width, &height, data.Data(), data.GetSize(), LCT_RGBA, 8);
	uint32 bufferSize = width * height * 4;

	if (buffer)
	{
		if (bufferSize)
		{
			outData = Buffer<byte>(buffer, bufferSize);
			outWidth = width;
			outHeight = height;
		}

		free(buffer);
		return true;
	}

	return false;
}

Texture* IO::ReadPNGFile(const Buffer<byte> &data)
{
	Texture* texture = nullptr;

	unsigned char *buffer;
	unsigned int width, height;

	lodepng_decode_memory(&buffer, &width, &height, data.Data(), data.GetSize(), LCT_RGBA, 8);
	uint32 bufferSize = width * height * 4;

	if (buffer)
	{
		if (bufferSize)
			texture = new Texture(Buffer<byte>(buffer, bufferSize), width, height);

		free(buffer);
	}

	return texture;
}
