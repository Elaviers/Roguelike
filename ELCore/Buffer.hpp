#pragma once
#include "FunctionPointer.hpp"
#include "Types.hpp"
#include "Utilities.hpp"
#include <initializer_list>
#include <utility>

template <typename T>
class Buffer
{
private:
	NewHandler _handlerNew;
	DeleteHandler _handlerDelete;

	union
	{
		T* _elements;
		byte* _data;
	};

	size_t _size;

	byte* _CreateNewArray(size_t newSize) const 
	{
		if (_handlerNew.IsCallable())
			return _handlerNew(sizeof(T) * newSize);

		return new byte[sizeof(T) * newSize]; 
	}

	void _DeleteData()
	{
		if (_handlerDelete.IsCallable())
			_handlerDelete(_data);
		else
			delete[] _data;
	}

public:
	Buffer() : _data(nullptr), _size(0) {}

	Buffer(const T& v) : _size(1)
	{
		_data = _CreateNewArray(_size);
		new (_elements) T(v);
	}

	Buffer(const std::initializer_list<T>& elements) : _size(elements.size())
	{
		_data = _CreateNewArray(_size);

		for (size_t i = 0; i < _size; ++i)
			new (_elements + i) T(elements.begin()[i]);
	}

	Buffer(const Buffer& other) : _size(other._size)
	{
		_data = _CreateNewArray(_size);

		for (size_t i = 0; i < _size; ++i)
			new (_elements + i) T(other[i]);
	}

	Buffer(const T* data, size_t size) : _size(size) 
	{
		_data = _CreateNewArray(_size);

		for (size_t i = 0; i < _size; ++i)
			new (_elements + i) T(data[i]);
	}

	Buffer(const NewHandler& newHandler, const DeleteHandler& deleteHandler) : Buffer(), _handlerNew(newHandler), _handlerDelete(deleteHandler) {}

	Buffer(Buffer&& other) noexcept : _data(other._data), _size(other._size)
	{
		other._data = nullptr;
		other._size = 0;

		if (other._handlerNew != _handlerNew || other._handlerDelete != _handlerDelete)
			operator=((const Buffer&)*this);
	}

	~Buffer() 
	{
		for (size_t i = 0; i < _size; ++i)
			_elements[i].~T();

		_DeleteData();
	}

	size_t GetSize() const	{ return _size; }
	T* Data()				{ return _elements; }
	const T* Data() const	{ return _elements; }

	T& operator[](size_t index)				{ return _elements[index]; }
	const T& operator[](size_t index) const	{ return _elements[index]; }

	T& Last() { return _elements[_size - 1]; }
	const T& Last() const { return _elements[_size - 1]; }

	void SetSize(size_t size)
	{
		if (_size == size)
			return;

		byte* newData;
		if (size != 0)
		{
			for (size_t i = size; i < _size; ++i)
				_elements[i].~T();

			newData = _CreateNewArray(size);
			auto minSize = size < _size ? size : _size;
			Utilities::CopyBytes(_data, newData, minSize * sizeof(T));

			for (size_t i = minSize; i < size; ++i)
				new (newData + sizeof(T) * i) T();
		}
		else
			newData = nullptr;

		_DeleteData();
		_data = newData;
		_size = size;
	}

	T& Add(const T& item)
	{
		SetSize(_size + 1);
		return *new (_elements + _size - 1) T(item);
	}

	T& Add(T&& item)
	{
		SetSize(_size + 1);
		return *new (_elements + _size - 1) T(std::move(item));
	}

	template <typename... Args>
	T& Emplace(const Args&... args)
	{
		SetSize(_size + 1);
		return *new (_elements + _size - 1) T(args...);
	}

	void Clear() { SetSize(0); }

	void Append(size_t elements) { SetSize(_size + elements); }

	T* Insert(const T &item, size_t pos)
	{
		if (pos > _size) return nullptr;

		byte *newData = _CreateNewArray(_size + 1);
		Utilities::CopyBytes(_data, newData, pos * sizeof(T));
		Utilities::CopyBytes(_data + pos * sizeof(T), newData + (pos + 1) * sizeof(T), (_size - pos) * sizeof(T));

		new (newData + sizeof(T) * pos) T(item);
		
		_DeleteData();
		_data = newData;
		_size++;

		return &_elements[pos];
	}

	T& OrderedAdd(const T &item)
	{
		for (size_t i = 0; i < _size; ++i)
			if (_elements[i] > item)
				return *Insert(item, i);

		return Add(item);
	}

	void RemoveIndex(size_t index)
	{
		if (index < _size)
		{
			_size--;
			_elements[index].~T();

			byte* newData = _CreateNewArray(_size);
			Utilities::CopyBytes(_data, newData, index * sizeof(T));
			Utilities::CopyBytes(_data + (index + 1) * sizeof(T), newData + index * sizeof(T), (_size - index) * sizeof(T));

			_DeleteData();
			_data = newData;
		}
	}

	void Remove(const T &item)
	{
		for (size_t i = 0; i < _size;)
		{
			if (_elements[i] == item)
				RemoveIndex(i);
			else
				++i;
		}
	}

	Buffer operator+(const T& other)
	{
		Buffer result;
		result._size = _size + 1;
		result._data = _CreateNewArray(result._size);
		
		for (size_t i = 0; i < _size; ++i)
			new (result._elements + i) T(_elements[i]);

		new (result._elements + _size) T(other);
		return result;
	}

	Buffer operator+(const Buffer& other) const
	{
		Buffer result;
		result._size = _size + other._size;
		result._data = _CreateNewArray(result._size);

		for (size_t i = 0; i < _size; ++i)
			new (result._elements + i) T(_elements[i]);

		for (size_t i = 0; i < other._size; ++i)
			new (result._elements + _size + i) T(other._elements[i]);

		return result;
	}

	Buffer& operator+=(const Buffer& other)
	{
		return *this = *this + other;
	}

	Buffer& operator=(const Buffer& other)
	{
		_DeleteData();
		_size = other._size;
		_data = _CreateNewArray(_size);

		for (size_t i = 0; i < _size; ++i)
			new (_elements + i) T(other[i]);

		return *this;
	}

	Buffer& operator=(Buffer&& other) noexcept
	{
		_data = other._data;
		_size = other._size;
		other._data = nullptr;
		other._size = 0;

		if (other._handlerNew != _handlerNew || other._handlerDelete != _handlerDelete)
			operator=((const Buffer&)*this);

		return *this;
	}

	bool operator==(const Buffer& other) const
	{
		if (_size != other._size) return false;

		for (size_t i = 0; i < _size; ++i)
			if (_elements[i] != other[i])
				return false;

		return true;
	}

	int IndexOf(const T& item) const
	{
		for (size_t i = 0; i < _size; ++i)
			if (_elements[i] == item)
				return (int)i;

		return -1;
	}

	//For range-based for
	T* begin() { return _elements; }
	const T* begin() const { return _elements; }
	T* end() { return _size ? (_elements + _size) : nullptr; }
	const T* end() const { return _size ? (_elements + _size) : nullptr; }
};
