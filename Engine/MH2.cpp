#include "Hashing.hpp"

uint32 Hashing::MurmurHash2(const void* ptr, size_t len, uint32 seed)
{
	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	uint32 h = seed ^ (uint32)len;

	const byte* data = (const byte*)ptr;

	while (len >= 4)
	{
		uint32 k = *(uint32*)data;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

	switch (len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
		h *= m;
	};

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}
