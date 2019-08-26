#pragma once
#include "Types.hpp"

typedef uint32 StackFrameSize;

class StackAllocator
{
	size_t _allocedSize;
	size_t _pageSize;
	size_t _currentIndex;

	byte* _data;

public:
	StackAllocator(size_t pageSize) : _allocedSize(pageSize), _pageSize(pageSize), _currentIndex(0) { _data = new byte[_pageSize]; }
	~StackAllocator() { delete[] _data; }
	
	inline byte* GetFirstFrame() { return _currentIndex ? _data : nullptr; }

	byte* Push(StackFrameSize size);

	template <typename T>
	inline T* Push() { return (T*)Push(sizeof(T)); }

	void Pop();

	inline void Reset() { _currentIndex = 0; }

	inline static byte GetOverheadBytesPerFrame() { return sizeof(StackFrameSize); }
};
