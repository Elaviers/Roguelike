#pragma once
#include "Buffer.hpp"
#include "Mesh_Static.hpp"
#include "Pair.hpp"
#include "Texture.hpp"

class WaveSound;

class Skeleton;

namespace IO {
	bool FileExists(const char* filename);

	Buffer<byte> ReadFile(const char *filename, bool silent = false);
	bool WriteFile(const char *filename, const byte *data, size_t dataLength);
	inline bool WriteFile(const char* filename, const Buffer<byte>& buffer) { return WriteFile(filename, buffer.Data(), buffer.GetSize()); }

	inline String ReadFileString(const char* filename, bool silent = false)
	{
		return String((char*)(ReadFile(filename, silent) + '\0').Data());
	}

	WaveSound* ReadWaveFile(const char* filename);

	bool ReadPNGFile(const Buffer<byte>& data, Buffer<byte>& outData, unsigned int& outWidth, unsigned int& outHeight);
	Texture* ReadPNGFile(const Buffer<byte> &data);
	
	inline bool ReadPNGFile(const char* filename, Buffer<byte>& outData, unsigned int& outWidth, unsigned int& outHeight) { return ReadPNGFile(IO::ReadFile(filename), outData, outWidth, outHeight); }
	inline Texture* ReadPNGFile(const char* filename) { return ReadPNGFile(IO::ReadFile(filename)); }
	
	Mesh_Static* ReadOBJFile(const char *filename);

	//searchPath must have a wildcard in it, like C:/Example/*.poo
	Buffer<String> FindFilesInDirectory(const char *searchPath);
	Buffer<String> FindFilesInDirectoryRecursive(const char *searchPath, const char *wildcard = "*.*");

	String OpenFileDialog(const wchar_t* dir, const Buffer<Pair<const wchar_t*>>& ext);
	String SaveFileDialog(const wchar_t* dir, const Buffer<Pair<const wchar_t*>>& ext);
}
