#pragma once
#include "Buffer.hpp"

class ByteWriter
{
	Buffer<byte>& _buffer;
	size_t _index;
	byte* _pointer;

public:
	ByteWriter(Buffer<byte>& buffer, size_t index = 0) : _buffer(buffer), _pointer(nullptr) { if (!SetIndex(index)) _index = 0; }

	byte* Ptr() { return _pointer; }
	bool IsValid() const { return _index < _buffer.GetSize(); }

	bool SetIndex(size_t index);
	bool IncrementIndex(size_t amount);
	size_t GetRemainingSpace();

	void EnsureSpace(size_t amount);
	void Write_byte(byte value);
	void Write_uint16(uint16 value);
	void Write_uint32(uint32 value);
	void Write_float(float value);

	//todo: double

	void Write_cstr(const char* string);

	template <typename T> void Write(T t) { t.Write(*this); }
	template <> void Write(byte x) { Write_byte(x); }
	template <> void Write(uint16 x) { Write_uint16(x); }
	template <> void Write(uint32 x) { Write_uint32(x); }
	template <> void Write(float x) { Write_float(x); }
	template <> void Write(const char* x) { Write_cstr(x); }
};

