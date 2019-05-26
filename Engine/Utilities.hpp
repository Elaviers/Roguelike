#pragma once
#include "String.hpp"
#include "Types.hpp"

namespace Utilities
{
	template <typename T>
	inline void Swap(T &a, T &b)
	{
		T temp = std::move(a);
		a = std::move(b);
		b = std::move(temp);
	}

	template <typename T>
	inline T Min(const T &a, const T &b) { return a < b ? a : b; }

	template <typename T>
	inline T Max(const T &a, const T &b) { return a > b ? a : b; }

	template <typename T>
	inline T Clamp(const T &x, const T &lower, const T &upper)
	{
		if (x < lower)
			return lower;
		if (x > upper)
			return upper;

		return x;
	}

	inline void CopyBytes(const void *src, void *dest, size_t length)
	{
		for (uint32 i = 0; i < length; ++i)
			reinterpret_cast<byte*>(dest)[i] = reinterpret_cast<const byte*>(src)[i];
	}

	template<typename T>
	inline T* CopyOf(const T &src, size_t size)
	{
		T* ptr = reinterpret_cast<T*>(new byte[size]);
		CopyBytes(&src, ptr, size);
		return ptr;
	}

	inline String GetExtension(const String& string)
	{
		for (size_t i = string.GetLength() - 1; i > 0; --i)
			if (string[i] == '.')
				return string.SubString(i, string.GetLength());

		return String();
	}

	inline void StripExtension(String &string)
	{
		for (size_t i = string.GetLength() - 1; i > 0; --i)
			if (string[i] == '.')
			{
				string.SetLength(i);
				break;
			}
	}

	inline void LowerString(String &string)
	{
		for (unsigned int i = 0; i < string.GetLength(); ++i)
			if (string[i] >= 'A' && string[i] <= 'Z')
				string[i] += ('a' - 'A');
	}
}
