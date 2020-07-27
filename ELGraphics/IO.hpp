#pragma once
#include <ELSys/IO.hpp>
#include "Mesh_Static.hpp"
#include "Texture.hpp"

namespace IO
{
	bool ReadPNGFile(const Buffer<byte>& data, Buffer<byte>& outData, unsigned int& outWidth, unsigned int& outHeight);
	Texture* ReadPNGFile(const Buffer<byte> &data);

	inline bool ReadPNGFile(const char* filename, Buffer<byte>& outData, unsigned int& outWidth, unsigned int& outHeight) { return ReadPNGFile(IO::ReadFile(filename), outData, outWidth, outHeight); }
	inline Texture* ReadPNGFile(const char* filename) { return ReadPNGFile(IO::ReadFile(filename)); }

	Mesh_Static* ReadOBJFile(const String& textData);
	inline Mesh_Static* ReadOBJFile(const char* filename) { return ReadOBJFile(IO::ReadFileString(filename)); }
}
