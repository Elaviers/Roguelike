#pragma once
#include "Types.hpp"

namespace Hashing
{
	void HashSha256(const void* data, size_t length, byte out[32]);

	uint32 MurmurHash2(const void* data, size_t length, uint32 seed = 0x6942);

}
