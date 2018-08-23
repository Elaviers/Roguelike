#pragma once
#include "Types.h"

template<typename T>
class Buffer
{
private:
	T *_data;
	uint32 _size;

public:
	Buffer() : _data(nullptr), _size(0) {}
	~Buffer() { delete[] _data; }

	Buffer(const Buffer& buffer)
	{
		_size = buffer._size;
		_data = new T[_size];
		
		for (uint32 i = 0; i < _size; ++i)
			_data[i] = buffer._data[i];
	}

	Buffer(Buffer&& buffer) : _data(buffer._data), _size(buffer._size) { buffer._data = nullptr; }

	void SetSize(uint32 size)
	{
		T *newData = new T[size];
		auto minSize = size < _size ? size : _size;
		for (uint32 i = 0; i < minSize; ++i)
			newData[i] = _data[i];

		delete[] _data;
		_data = newData;
		_size = size;
	}

	inline void Append(uint32 elements) { SetSize(_size + elements); }

	inline uint32 GetSize() const { return _size; }
	inline T* const Data() const { return _data; }

	inline T&		operator[](uint32 index)		{ return _data[index]; }
	inline const T& operator[](uint32 index) const	{ return _data[index]; }

	inline Buffer&	operator=(Buffer &&other) { _data = other._data; _size = other._size; other._data = nullptr; other._size = 0; return *this; }

	void Add(const T &item)
	{
		SetSize(_size + 1);

		_data[_size - 1] = item;
	}

	void Insert(const T &item, uint32 pos)
	{
		if (pos > _size) return;

		T *newData = new T[_size + 1];
		for (uint32 i = 0; i < pos; ++i)
			newData[i] = _data[i];

		newData[pos] = item;

		for (uint32 i = pos; i < _size; ++i)
			newData[i + 1] = _data[i];

		delete[] _data;
		_data = newData;
		_size++;
	}

	inline void _SetRaw(T *data, uint32 size)
	{ 
		if (_size) delete[] _data;

		_data = data; _size = size;
	}


	inline		 T& Last()		 { return _data[_size - 1]; }
	inline const T& Last() const { return _data[_size - 1]; }
};
