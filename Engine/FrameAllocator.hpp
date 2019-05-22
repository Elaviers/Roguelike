#pragma once
#include "Types.hpp"

class FrameAllocator
{
	size_t _allocedSize;
	size_t _pageSize;
	size_t _usedSize;

	byte* _data;

public:
	FrameAllocator(float pageSize) : _allocedSize(pageSize), _pageSize(pageSize), _usedSize(0) { _data = new byte[_pageSize]; }
	~FrameAllocator() { delete[] _data; }

	byte* Allocate(size_t size);

	inline void Reset() { _usedSize = 0; }
};
