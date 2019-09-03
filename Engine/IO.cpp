#include "IO.hpp"
#include "Debug.hpp"
#include "lodepng.h"
#include "Utilities.hpp"
#include <Windows.h>

bool IO::FileExists(const char* filename)
{
	WIN32_FIND_DATAA fd;

	HANDLE file = ::FindFirstFileA(filename, &fd);

	return file != INVALID_HANDLE_VALUE;
}

Buffer<byte> IO::ReadFile(const char *filename, bool silent)
{
	Buffer<byte> buffer;
	
	HANDLE file = ::CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER fileSize;
		::GetFileSizeEx(file, &fileSize);
		buffer.SetSize(fileSize.LowPart);

		#pragma warning(suppress: 28193) //False positive
		BOOL success = ::ReadFile(file, buffer.Data(), fileSize.LowPart, NULL, NULL);

		if (!silent && success == FALSE)
			Debug::Error(CSTR("Could not read file \"", filename, '\"'));

		::CloseHandle(file);
	}
	else if (!silent)
		Debug::Error(CSTR("Could not open file \"", filename, '\"'));

	return buffer;
}

bool IO::WriteFile(const char *filename, const byte *data, size_t length)
{
	HANDLE file = ::CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return false;

	BOOL success = ::WriteFile(file, data, (DWORD)length, NULL, NULL);
	::CloseHandle(file);

	return success;
}

Texture* IO::ReadPNGTexture(const Buffer<byte> &data)
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

Buffer<String> IO::FindFilesInDirectory(const char *search)
{
	Buffer<String> filenames;

	WIN32_FIND_DATAA data;
	HANDLE hFile = ::FindFirstFileA(search, &data);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			filenames.Add(data.cFileName);
		} while (::FindNextFile(hFile, &data));
	}

	return filenames;
}
