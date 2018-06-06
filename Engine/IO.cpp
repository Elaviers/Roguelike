#include "IO.h"
#include "Error.h"
#include "lodepng.h"
#include "Utilities.h"
#include <Windows.h>

Buffer<byte> IO::ReadFile(const char *filename) {
	Buffer<byte> buffer;
	
	HANDLE file = ::CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER fileSize;
		::GetFileSizeEx(file, &fileSize);
		buffer.SetSize(fileSize.LowPart);

		::ReadFile(file, buffer.Data(), fileSize.LowPart, NULL, NULL);
		::CloseHandle(file);
	}

	return buffer;
}

bool IO::WriteFile(const char *filename, const byte *data, uint32 length)
{
	HANDLE file = ::CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return false;

	BOOL success = ::WriteFile(file, data, length, NULL, NULL);
	::CloseHandle(file);

	return success;
}

String IO::ReadFileString(const char *filename)
{
	Buffer<byte> buffer = ReadFile(filename);
	String string(buffer.GetSize());

	for (uint32 i = 0; i < string.GetLength(); ++i)
		string[i] = buffer[i];

	return string;
}

#include <vector> //For picopng
using std::vector;

TextureData IO::ReadPNGFile(const char *filename)
{
	TextureData out = {};

	Buffer<byte> fileBuffer = IO::ReadFile(filename);

	if (fileBuffer.GetSize())
	{
		unsigned char *buffer;
		unsigned int width, height;

		lodepng_decode_memory(&buffer, &width, &height, fileBuffer.Data(), fileBuffer.GetSize(), LCT_RGBA, 8);
		uint32 bufferSize = width * height * (4 * 8);

		out.width = width;
		out.height = height;
		out.data._SetRaw(buffer, bufferSize);
	}
	else Error(CSTR("Could not open file \"" + filename + '\"'));

	return out;
}
