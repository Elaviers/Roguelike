#pragma once
#include <cstdint>

typedef int8_t	int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t	 byte;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

enum class EDirection {RIGHT, UP, FORWARD};

struct Float_IEEE754
{
	bool sign : 1;
	byte exponent : 8;
	unsigned int mantissa : 23;
};
