#include "StackAllocator.hpp"
#include "Utilities.hpp"

byte* StackAllocator::Push(StackFrameSize size)
{
	auto requiredBytes = size + sizeof(StackFrameSize);

	while (_currentIndex + requiredBytes > _allocedSize)
	{
		byte* newData = new byte[_allocedSize + _pageSize];
		
		Utilities::CopyBytes(_data, newData, _currentIndex);
		delete[] _data;
		_data = newData;
		_allocedSize += _pageSize;
	}

	byte* addr = _data + _currentIndex;
	Utilities::TransferBE(size, addr + size);

	_currentIndex += requiredBytes;
	return addr;
}

void StackAllocator::Pop()
{
	if (_currentIndex)
		_currentIndex -= Utilities::TransferBE<uint32>(_data + _currentIndex - sizeof(StackFrameSize));
}
