#pragma once
#include "Types.h"

namespace Utilities
{
	template <typename T>
	inline void Swap(T &a, T &b)
	{
		T temp = a;
		a = b;
		b = temp;
	}

	template <typename T>
	inline T Min(const T &a, const T &b) { return a < b ? a : b; }

	template <typename T>
	inline T Max(const T &a, const T &b) { return a > b ? a : b; }

	inline void CopyBytes(const void *src, void *dest, uint32 length)
	{
		for (uint32 i = 0; i < length; ++i)
			reinterpret_cast<byte*>(dest)[i] = reinterpret_cast<const byte*>(src)[i];
	}

	inline void StripExtension(String &string)
	{
		for (unsigned int i = string.GetLength() - 1; i > 0; --i)
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
