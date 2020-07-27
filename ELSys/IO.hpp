#pragma once
#include <ELCore/Buffer.hpp>
#include <ELCore/Pair.hpp>

namespace IO {
	bool FileExists(const char* filename);

	Buffer<byte> ReadFile(const char *filename, bool silent = false);
	bool WriteFile(const char *filename, const byte *data, size_t dataLength);
	inline bool WriteFile(const char* filename, const Buffer<byte>& buffer) { return WriteFile(filename, buffer.Data(), buffer.GetSize()); }

	inline String ReadFileString(const char* filename, bool silent = false)
	{
		return String((char*)(ReadFile(filename, silent) + '\0').Data());
	}

	//searchPath must have a wildcard in it, like C:/Example/*.poo
	Buffer<String> FindFilesInDirectory(const char *searchPath);
	Buffer<String> FindFilesInDirectoryRecursive(const char *searchPath, const char *wildcard = "*.*");

	String OpenFileDialog(const wchar_t* dir, const Buffer<Pair<const wchar_t*>>& ext);
	String SaveFileDialog(const wchar_t* dir, const Buffer<Pair<const wchar_t*>>& ext);
}
