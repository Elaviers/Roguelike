#pragma once
#include "Buffer.hpp"
#include "String.hpp"

struct Float_IEEE754
{
	bool sign : 1;
	byte exponent : 8;
	unsigned int mantissa : 23;
};

template <typename T>
class BufferIterator
{
	Buffer<T> &_buffer;
	size_t _index;
	T *_pointer;

public:
	BufferIterator(Buffer<T> &buffer, size_t index = 0) : _buffer(buffer), _pointer(nullptr) { if (!SetIndex(index)) _index = 0; }
	BufferIterator() {}

	inline T* Ptr() { return _pointer; }

	inline bool Valid() const { return _index < _buffer.GetSize(); }

	inline bool SetIndex(size_t index)
	{
		if (index < _buffer.GetSize())
		{
			_index = index;
			_pointer = &_buffer[_index];
			return true;
		}

		return false;
	}

	bool StepForward(bool createNew, size_t amount = 1)
	{
		if (SetIndex(_index + amount))
			return true;

		if (createNew)
		{
			size_t newSlotsNeeded;

			if (_buffer.GetSize() == 0)
				newSlotsNeeded = amount;
			else
				newSlotsNeeded = amount - (_buffer.GetSize() - 1 - _index);

			_buffer.Append(newSlotsNeeded);
			_index = _buffer.GetSize() - 1;
			_pointer = &_buffer[_index];
			return true;
		}
		else
		{
			_index += amount;
			_pointer = nullptr;
		}

		return false;
	}

	inline T* operator*() { return _pointer; }
	inline T* operator->() { return _pointer; }


	void Write_byte(byte value)
	{
		StepForward(true);
		*_pointer = value;
	}

	void Write_uint16(uint16 value)
	{
		byte low = value & 0x00FF;
		byte high = (value & 0xFF00) >> 8;

		StepForward(true, 2);
		_pointer[-1] = high;
		_pointer[0] = low;
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

		StepForward(true, 4);
		_pointer[-3] = bytes[0];
		_pointer[-2] = bytes[1];
		_pointer[-1] = bytes[2];
		_pointer[0] = bytes[3];
	}

	void Write_string(const char *string)
	{
		size_t len = StringLength(string);

		StepForward(true, len + 1);

		_pointer[0] = '\0';

		for (unsigned int i = 1; i <= len; ++i)
			_pointer[-(int)i] = string[len - i];
	}

	//returns bytes read
	size_t ReadTo(T* dest, size_t amount)
	{
		for (size_t i = 0; i < amount; ++i)
		{
			dest[i] = *_pointer;

			if (!StepForward(false, 1))
				return i + 1;
		}

		return amount;
	}

	inline void Write_vector2(const Vector2 &v)
	{
		Write_float(v[0]);
		Write_float(v[1]);
	}

	inline void Write_vector3(const Vector3 &v)
	{
		Write_float(v[0]);
		Write_float(v[1]);
		Write_float(v[2]);
	}

	inline byte Read_byte()
	{
		byte value = *_pointer;
		StepForward(false, 1);
		return value;
	}

	uint16 Read_uint16()
	{
		uint16 value = (_pointer[0] << 8) + _pointer[1];
		StepForward(false, 2);
		return value;
	}

	uint16 Read_uint16_little()
	{
		uint16 value = _pointer[0] + (_pointer[1] << 8);
		StepForward(false, 2);
		return value;
	}

	uint32 Read_uint32_little()
	{
		uint32 value = _pointer[0] + (_pointer[1] << 8) + (_pointer[2] << 16) + (_pointer[3] << 24);
		StepForward(false, 4);
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

		StepForward(false, 4);
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
};
