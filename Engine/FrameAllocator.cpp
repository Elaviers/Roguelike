#include "FrameAllocator.hpp"
#include "Utilities.hpp"

byte* FrameAllocator::Allocate(size_t size)
{
	if (_usedSize + size > _allocedSize)
	{
		byte* newData = new byte[_allocedSize + _pageSize];
		
		Utilities::CopyBytes(_data, newData, _usedSize);
		delete[] _data;
		_data = newData;
	}

	byte* addr = _data + _usedSize;
	_usedSize += size;
	return addr;
}
