#include "ByteWriter.hpp"
#include "Types.hpp"

bool ByteWriter::SetIndex(size_t index)
{
	_index = index;

	if (_index < _buffer.GetSize())
	{
		_pointer = &_buffer[_index];
		return true;
	}

	_pointer = nullptr;
	return false;
}

bool ByteWriter::IncrementIndex(size_t amount)
{
	return SetIndex(_index + amount);
}

size_t ByteWriter::GetRemainingSpace()
{
	return _buffer.GetSize() == 0 ? 0 : (_buffer.GetSize() - _index);
}

void ByteWriter::EnsureSpace(size_t amount)
{
	size_t remainingSpace = GetRemainingSpace();

	if (amount > remainingSpace)
		_buffer.Append(amount - remainingSpace);

	_pointer = &_buffer[_index];
}

void ByteWriter::Write_byte(byte value)
{
	EnsureSpace(1);
	*_pointer = value;
	IncrementIndex(1);
}

void ByteWriter::Write_uint16(uint16 value)
{
	EnsureSpace(2);
	_pointer[0] = (value & 0xFF00) >> 8;
	_pointer[1] = (value & 0x00FF);
	IncrementIndex(2);
}

void ByteWriter::Write_uint32(uint32 value)
{
	EnsureSpace(4);
	_pointer[0] = (value & 0xFF000000) >> 24;
	_pointer[1] = (value & 0x00FF0000) >> 16;
	_pointer[2] = (value & 0x0000FF00) >> 8;
	_pointer[3] = (value & 0x000000FF);
	IncrementIndex(4);
}

void ByteWriter::Write_float(float value)
{
	//Todo- Proper IEEE 754 conversion

	union
	{
		Float_IEEE754 data;

		byte bytes[4];
		float test;
	};

	/*
	sign = value < 0.f;
	exponent = 127;
	mantissa = 0;

	uint32 valueInt = (uint32)Maths::Abs(value);

	if (value != valueInt)
	{
		do
		{
			exponent--;
			value *= 2.f;
		} while (value != valueInt);
	}
	else
	{
		do
		{
			exponent++;
			value /= 2.f;
		} while ()
	}
	*/

	test = value;

	EnsureSpace(4);
	_pointer[0] = bytes[0];
	_pointer[1] = bytes[1];
	_pointer[2] = bytes[2];
	_pointer[3] = bytes[3];
	IncrementIndex(4);
}

//todo: double

void ByteWriter::Write_cstr(const char* string)
{
	size_t len = StringLength(string);

	EnsureSpace(len + 1);

	_pointer[len] = '\0';

	for (unsigned int i = 0; i < len; ++i)
		_pointer[i] = string[i];

	IncrementIndex(len + 1);
}
