#pragma once
#include "Buffer.hpp"
#include "Mesh_Static.hpp"

struct WaveSound;

class Skeleton;

struct TextureData
{
	Buffer<byte> data;
	uint32 width;
	uint32 height;

	inline bool IsValid() { return data.GetSize() != 0; }
};

namespace IO {
	namespace DataIDs
	{
		enum
		{
			MESH_STATIC = 0,
			MESH_SKELETAL = 1,
			TEXTURE = 10
		};
	}

	bool FileExists(const char* filename);

	Buffer<byte> ReadFile(const char *filename);
	bool WriteFile(const char *filename, const byte *data, size_t dataLength);

	bool WriteMesh(const char* filename, const Mesh* mesh);
	Mesh* ReadMesh(const char* filename);

	String ReadFileString(const char *filename);

	WaveSound ReadWaveFile(const char* filename);

	TextureData ReadPNGFile(const char *filename);
	Mesh_Static* ReadOBJFile(const char *filename);

	//searchPath must have a wildcard in it, like C:/Example/*.poo
	Buffer<String> FindFilesInDirectory(const char *searchPath);
}
