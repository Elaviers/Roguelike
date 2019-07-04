#pragma once
#include "Buffer.hpp"
#include "Mesh_Static.hpp"
#include "Texture.hpp"

struct WaveSound;

class Skeleton;

namespace IO {
	bool FileExists(const char* filename);

	Buffer<byte> ReadFile(const char *filename);
	bool WriteFile(const char *filename, const byte *data, size_t dataLength);
	inline bool WriteFile(const char* filename, const Buffer<byte>& buffer) { return WriteFile(filename, buffer.Data(), buffer.GetSize()); }

	String ReadFileString(const char *filename);

	WaveSound* ReadWaveFile(const char* filename);

	Texture* ReadPNGTexture(const Buffer<byte> &data);
	inline Texture* ReadPNGTexture(const char* filename) { return ReadPNGTexture(IO::ReadFile(filename)); }
	Mesh_Static* ReadOBJFile(const char *filename);

	//searchPath must have a wildcard in it, like C:/Example/*.poo
	Buffer<String> FindFilesInDirectory(const char *searchPath);
}
