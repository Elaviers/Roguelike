#pragma once
#include "Sound.h"
#include "Bounds.h"
#include "Buffer.h"
#include "String.h"
#include "Vertex.h"

struct TextureData
{
	Buffer<byte> data;
	uint32 width;
	uint32 height;

	inline bool IsValid() { return data.GetSize() != 0; }
};

struct ModelData
{
	Buffer<Vertex17F> vertices;
	Buffer<uint32> elements;
	Bounds bounds;

	inline bool IsValid() { return vertices.GetSize() != 0; }
};

namespace IO {
	bool FileExists(const char* filename);

	Buffer<byte> ReadFile(const char *filename);
	bool WriteFile(const char *filename, const byte *data, uint32 dataLength);

	String ReadFileString(const char *filename);

	WaveSound ReadWaveFile(const char* filename);

	TextureData ReadPNGFile(const char *filename);
	ModelData ReadOBJFile(const char *filename);

	//searchPath must have a wildcard in it, like C:/Example/*.poo
	Buffer<String> FindFilesInDirectory(const char *searchPath);
}
