#pragma once
#include "Types.hpp"
#include "Utilities.hpp"
#include <initializer_list>
#include <utility>

template <typename T>
class Buffer
{
private:
	T *_data;
	size_t _size;

public:
	Buffer() : _data(nullptr), _size(0) {}

	Buffer(std::initializer_list<T> array) : _size(array.size())
	{
		_data = new T[_size];

		for (size_t i = 0; i < _size; ++i)
			_data[i] = array.begin()[i];
	}

	Buffer(T* data, size_t size) : _data(Utilities::CopyOfArray(data, size)), _size(size) {}

	~Buffer() { delete[] _data; }

	Buffer(const Buffer& other) : _size(other._size), _data(new T[other._size])
	{
		for (size_t i = 0; i < _size; ++i)
			_data[i] = other._data[i];
	}

	Buffer(Buffer&& buffer) noexcept : _data(buffer._data), _size(buffer._size) { buffer._data = nullptr; }

	void SetSize(size_t size)
	{
		T *newData = new T[size];
		auto minSize = size < _size ? size : _size;
		for (size_t i = 0; i < minSize; ++i)
			newData[i] = std::move(_data[i]);

		delete[] _data;
		_data = newData;
		_size = size;
	}

	inline void Clear() { SetSize(0); }

	inline void Append(size_t elements) { SetSize(_size + elements); }

	Buffer& operator=(const Buffer &other)
	{
		delete[] _data;
		_size = other._size;
		_data = new T[_size];

		for (size_t i = 0; i < _size; ++i)
			_data[i] = other._data[i];

		return *this;
	}

	inline Buffer& operator=(Buffer &&other) noexcept 
	{ 
		_data = other._data;
		_size = other._size;
		other._data = nullptr;
		other._size = 0;

		return *this; 
	}

	T& Add(const T &item)
	{
		SetSize(_size + 1);

		return _data[_size - 1] = item;
	}

	T& Add(T &&item)
	{
		SetSize(_size + 1);

		return _data[_size - 1] = std::move(item);
	}

	T* Insert(const T &item, size_t pos)
	{
		if (pos > _size) return nullptr;

		T *newData = new T[_size + 1];
		for (size_t i = 0; i < pos; ++i)
			newData[i] = std::move(_data[i]);

		newData[pos] = item;

		for (size_t i = pos; i < _size; ++i)
			newData[i + 1] = std::move(_data[i]);

		delete[] _data;
		_data = newData;
		_size++;

		return &newData[pos];
	}

	void RemoveIndex(size_t index)
	{
		if (index < _size)
		{
			_size--;

			T* newData = new T[_size];
			for (size_t i = 0; i < index; ++i)
#pragma warning(suppress: 6386) //False positive
				newData[i] = std::move(_data[i]);

			for (size_t i = index; i < _size; ++i)
				newData[i] = std::move(_data[i + 1]);

			delete[] _data;
			_data = newData;
		}
	}

	void Remove(const T &item)
	{
		for (size_t i = 0; i < _size;)
		{
			if (_data[i] == item)
				RemoveIndex(i);
			else
				++i;
		}
	}

	inline size_t GetSize() const					{ return _size; }
	inline		 T* Data()							{ return _data; }
	inline const T* Data() const					{ return _data; }
	inline		 T& Last()							{ return _data[_size - 1]; }
	inline const T& Last() const					{ return _data[_size - 1]; }

	inline		 T&	operator[](size_t index)		{ return _data[index]; }
	inline const T& operator[](size_t index) const	{ return _data[index]; }

	Buffer operator+(const Buffer &other) const
	{
		Buffer result;
		result.SetSize(_size + other._size);

		for (size_t i = 0; i < _size; ++i)
			result._data[i] = _data[i];
		
		for (size_t i = 0; i < other._size; ++i)
			result._data[_size + i] = other._data[i];

		return result;
	}

	bool operator==(const Buffer& other) const
	{
		if (_size != other._size) return false;

		for (size_t i = 0; i < _size; ++i)
			if (_data[i] != other._data[i])
				return false;

		return true;
	}

	void Shuffle()
	{
		for (size_t i = 0; i < _size; ++i)
			Utilities::Swap(_data[i], _data[Maths::Random() * _size]);
	}
};
