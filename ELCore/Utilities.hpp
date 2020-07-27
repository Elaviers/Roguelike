#pragma once
#include "String.hpp"
#include "Types.hpp"
#include <utility>

namespace Utilities
{
	template <typename T>
	inline void Swap(T &a, T &b)
	{
		T temp = std::move(a);
		a = std::move(b);
		b = std::move(temp);
	}

	inline void CopyBytes(const void *src, void *dest, size_t length)
	{
		for (size_t i = 0; i < length; ++i)
			reinterpret_cast<byte*>(dest)[i] = reinterpret_cast<const byte*>(src)[i];
	}

	template <typename T>
	inline void CopyArray(const T* src, T* dest, size_t length)
	{
		for (size_t i = 0; i < length; ++i)
			dest[i] = src[i];
	}

	template<typename T>
	inline T* CopyOf(const T &src, size_t size)
	{
		T* ptr = reinterpret_cast<T*>(new byte[size]);
		CopyBytes(&src, ptr, size);
		return ptr;
	}

	template <typename T>
	inline T* CopyOfArray(const T* src, size_t length)
	{
		T* dest = new T[length];
		CopyArray(src, dest, length);
		return dest;
	}

	inline String GetExtension(const String& string)
	{
		for (size_t i = string.GetLength() - 1; i > 0; --i)
			if (string[i] == '.')
				return string.SubString(i, string.GetLength());

		return String();
	}

	inline String WithCarriageReturns(const String& string)
	{
		String result(string);

		for (size_t i = 0; i < result.GetLength(); )
		{
			if (result[i] == '\n')
			{
				if (i <= 0 || result[i - 1] != '\r')
				{
					result.Insert('\r', i);
					i += 2;
					continue;
				}
			}

			++i;
		}

		return result;
	}

	inline void StripExtension(String &string)
	{
		for (size_t i = string.GetLength() - 1; i > 0; --i)
			if (string[i] == '.')
			{
				string.ShrinkTo(i);
				break;
			}
	}

	inline void LowerString(String &string)
	{
		for (unsigned int i = 0; i < string.GetLength(); ++i)
			if (string[i] >= 'A' && string[i] <= 'Z')
				string[i] += ('a' - 'A');
	}

	template <typename T>
	inline void TransferLE(T from, byte *to)
	{
		auto sz = sizeof(T);
		T mask = 0xFF;
		for (byte i = 0; i < sz; ++i)
		{
			to[i] = (from & mask) >> (8 * i);
			mask <<= 8;
		}
	}

	template <typename T>
	inline void TransferBE(T from, byte *to)
	{
		auto sz = sizeof(T);
		T mask = 0xFF;
		for (byte i = 0; i < sz; ++i)
		{
			to[sz - 1 - i] = (from & mask) >> (8 * i);
			mask <<= 8;
		}
	}

	template <typename T>
	inline T TransferLE(byte* from)
	{
		T result = 0;
		byte shift = 8 * (sizeof(T) - 1);
		for (byte i = 0; i < sizeof(T); ++i)
		{
			result += from[i] << shift;
			shift -= 8;
		}
		return result;
	}

	template <typename T>
	inline T TransferBE(byte *from)
	{
		T result = 0;
		byte shift = 0;
		for (byte i = 0; i < sizeof(T); ++i)
		{
			result += from[i] << shift;
			shift += 8;
		}
		return result;
	}
}
