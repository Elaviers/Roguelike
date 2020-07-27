#pragma once
#include "Buffer.hpp"

class ByteReader
{
	const Buffer<byte> &_buffer;
	size_t _index;
	const byte* _pointer;

public:
	ByteReader(const Buffer<byte>& buffer, size_t index = 0) : _buffer(buffer), _pointer(nullptr) { if (!SetIndex(index)) _index = 0; }

	const byte* Ptr() const { return _pointer; }
	bool IsValid() const { return _index < _buffer.GetSize(); }

	bool SetIndex(size_t index);
	bool IncrementIndex(size_t amount);
	size_t GetRemainingSpace();

	//returns bytes read
	size_t Read(byte* dest, size_t amount);

	byte Read_byte();
	uint16 Read_uint16();
	uint16 Read_uint16_little();
	uint32 Read_uint32();
	uint32 Read_uint32_little();
	float Read_float();

	template <typename T> T Read() { T t; t.Read(*this); return t; }
	template <> byte Read() { return Read_byte(); }
	template <> uint16 Read() { return Read_uint16(); }
	template <> uint32 Read() { return Read_uint32(); }
	template <> float Read() { return Read_float(); }
};
