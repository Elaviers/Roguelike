#pragma once
#include "Sound.hpp"
#include "Bounds.hpp"
#include "Buffer.hpp"
#include "String.hpp"
#include "Vertex.hpp"

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
	bool WriteFile(const char *filename, const byte *data, size_t dataLength);

	bool WriteModelData(const char* filename, const ModelData& data);
	ModelData ReadModelData(const char* filename);

	String ReadFileString(const char *filename);

	WaveSound ReadWaveFile(const char* filename);

	TextureData ReadPNGFile(const char *filename);
	ModelData ReadOBJFile(const char *filename);

	//searchPath must have a wildcard in it, like C:/Example/*.poo
	Buffer<String> FindFilesInDirectory(const char *searchPath);
}
