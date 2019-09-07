#pragma once
#include "Buffer.hpp"
#include "String.hpp"
#include "Vector4.hpp"

struct Float_IEEE754
{
	bool sign : 1;
	byte exponent : 8;
	unsigned int mantissa : 23;
};

template <typename T>
class BufferReader
{
	const Buffer<T> &_buffer;
	size_t _index;
	const T* _pointer;

public:
	BufferReader(const Buffer<T>& buffer, size_t index = 0) : _buffer(buffer), _pointer(nullptr) { if (!SetIndex(index)) _index = 0; }

	inline const T* Ptr() const { return _pointer; }
	inline bool Valid() const { return _index < _buffer.GetSize(); }

	inline bool SetIndex(size_t index)
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

	inline bool IncrementIndex(size_t amount)
	{
		return SetIndex(_index + amount);
	}

	inline size_t GetRemainingSpace()
	{
		return _buffer.GetSize() == 0 ? 0 : (_buffer.GetSize() - _index);
	}

	//returns bytes read
	size_t Read(T* dest, size_t amount)
	{
		for (size_t i = 0; i < amount; ++i)
		{
			dest[i] = *_pointer;

			if (!IncrementIndex(1))
				return i + 1;
		}

		return amount;
	}

	inline byte Read_byte()
	{
		byte value = *_pointer;
		IncrementIndex(1);
		return value;
	}

	uint16 Read_uint16()
	{
		uint16 value = (_pointer[0] << 8) + _pointer[1];
		IncrementIndex(2);
		return value;
	}

	uint16 Read_uint16_little()
	{
		uint16 value = _pointer[0] + (_pointer[1] << 8);
		IncrementIndex(2);
		return value;
	}

	uint32 Read_uint32()
	{
		uint16 value = (_pointer[0] << 24) + (_pointer[1] << 16) + (_pointer[2] << 8) + _pointer[3];
		IncrementIndex(4);
		return value;
	}

	uint32 Read_uint32_little()
	{
		uint32 value = _pointer[0] + (_pointer[1] << 8) + (_pointer[2] << 16) + (_pointer[3] << 24);
		IncrementIndex(4);
		return value;
	}

	float Read_float()
	{
		union
		{
			//yucky stuff
			byte bytes[4];

			float value;
		};

		bytes[0] = _pointer[0];
		bytes[1] = _pointer[1];
		bytes[2] = _pointer[2];
		bytes[3] = _pointer[3];

		IncrementIndex(4);
		return value;
	}

	String Read_string()
	{
		String string;
		byte next;

		while (next = Read_byte())
			string += next;

		return string;
	}

	inline Vector2 Read_vector2()
	{
		Vector2 v;
		v[0] = Read_float();
		v[1] = Read_float();
		return v;
	}

	inline Vector3 Read_vector3()
	{
		Vector3 v;
		v[0] = Read_float();
		v[1] = Read_float();
		v[2] = Read_float();
		return v;
	}

	inline Vector4 Read_vector4()
	{
		Vector4 v;
		v[0] = Read_float();
		v[1] = Read_float();
		v[2] = Read_float();
		v[3] = Read_float();
		return v;
	}
};

template <typename T>
class BufferWriter
{
	Buffer<T> &_buffer;
	size_t _index;
	T *_pointer;

public:
	BufferWriter(Buffer<T> &buffer, size_t index = 0) : _buffer(buffer), _pointer(nullptr) { if (!SetIndex(index)) _index = 0; }

	inline T* Ptr() { return _pointer; }

	inline bool Valid() const { return _index < _buffer.GetSize(); }

	inline bool SetIndex(size_t index)
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

	inline bool IncrementIndex(size_t amount)
	{
		return SetIndex(_index + amount);
	}

	inline size_t GetRemainingSpace()
	{
		return _buffer.GetSize() == 0 ? 0 : (_buffer.GetSize() - _index);
	}

	void EnsureSpace(size_t amount)
	{
		size_t remainingSpace = GetRemainingSpace();

		if (amount > remainingSpace)
			_buffer.Append(amount - remainingSpace);

		_pointer = &_buffer[_index];
	}

	void Write_byte(byte value)
	{
		EnsureSpace(1);
		*_pointer = value;
		IncrementIndex(1);
	}

	void Write_uint16(uint16 value)
	{
		EnsureSpace(2);
		_pointer[0] = (value & 0xFF00) >> 8;
		_pointer[1] =  (value & 0x00FF);
		IncrementIndex(2);
	}

	void Write_uint32(uint32 value)
	{
		EnsureSpace(4);
		_pointer[0] = (value & 0xFF000000) >> 24;
		_pointer[1] = (value & 0x00FF0000) >> 16;
		_pointer[2] = (value & 0x0000FF00) >> 8;
		_pointer[3] =  (value & 0x000000FF);
		IncrementIndex(4);
	}

	void Write_float(float value)
	{
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

	void Write_string(const char *string)
	{
		size_t len = StringLength(string);

		EnsureSpace(len + 1);

		_pointer[len] = '\0';

		for (unsigned int i = 0; i < len; ++i)
			_pointer[i] = string[i];

		IncrementIndex(len + 1);
	}

	inline void Write_vector2(const Vector2& v)
	{
		Write_float(v[0]);
		Write_float(v[1]);
	}

	inline void Write_vector3(const Vector3& v)
	{
		Write_float(v[0]);
		Write_float(v[1]);
		Write_float(v[2]);
	}

	inline void Write_vector4(const Vector4& v)
	{
		Write_float(v[0]);
		Write_float(v[1]);
		Write_float(v[2]);
		Write_float(v[3]);
	}
};
